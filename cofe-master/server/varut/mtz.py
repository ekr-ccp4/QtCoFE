
import os
import struct
from   varut  import jsonut


class measured(jsonut.jObject):
   value = sigma = None

   def __init__(self, v, s):
      self.value = v
      self.sigma = s


class bfpair(jsonut.jObject):
   plus = minus = None

   def __init__(self, v, s):
      self.plus = measured(v[0], s[0])
      self.minus = measured(v[1], s[1])


class dataset(jsonut.jObject):
   H = K = L = FREE = HM = None
   Ipm = Fpm = Imean = Fmean = None
   MTZ = PROJECT = CRYSTAL = DATASET = DCELL = DWAVEL = None

   def __init__(self, columns, header):
      self.MTZ = header.MTZ
      self.H = header.H
      self.K = header.K
      self.L = header.L
      self.FREE = header.FREE
      self.HM = header.HM

      for type in 'K', 'M', 'G', 'L':
         labs = columns.pop(type, None)
         if labs and len(labs) == 2:
            lab1, lab2 = labs
            if lab1.replace('+', '-') == lab2 and lab2.replace('-', '+') == lab1:
               columns[type] = lab1, lab2

            if lab1.replace('-', '+') == lab2 and lab2.replace('+', '-') == lab1:
               columns[type] = lab2, lab1

      for vtype, stype, tag in (('K', 'M', 'Ipm'), ('G', 'L', 'Fpm')):
         vlabs = columns.pop(vtype, None)
         slabs = columns.get(stype, None)
         if vlabs and slabs:
            setattr(self, tag, bfpair(vlabs, slabs))

      slabs = columns.pop('Q', None)

      for vtype, tag in (('J', 'Imean'), ('F', 'Fmean')):
         vlabs = columns.pop(vtype, None)
         if vlabs and len(vlabs) == 1:
            vlab, = vlabs
            slab = 'SIG' + vlab
            if slab in slabs:
               setattr(self, tag, measured(vlab, slab))

   def prn(self):
      print
      print 'MTZ =', self.MTZ
      print 'PROJECT/CRYSTAL/DATASET = %s/%s/%s' %(self.PROJECT, self.CRYSTAL, self.DATASET)
      print 'HM =', self.HM
      print 'DCELL =', self.DCELL
      print 'DWAVEL =', self.DWAVEL
      print 'H =', self.H
      print 'K =', self.K
      print 'L =', self.L
      print 'FREE =', self.FREE
      if self.Ipm is None:
         print 'Ipm =', self.Ipm

      else:
         print 'Ipm.plus.value =', self.Ipm.plus.value
         print 'Ipm.plus.sigma =', self.Ipm.plus.sigma
         print 'Ipm.minus.value =', self.Ipm.minus.value
         print 'Ipm.minus.sigma =', self.Ipm.minus.sigma

      if self.Fpm is None:
         print 'Fpm =', self.Fpm

      else:
         print 'Fpm.plus.value =', self.Fpm.plus.value
         print 'Fpm.plus.sigma =', self.Fpm.plus.sigma
         print 'Fpm.minus.value =', self.Fpm.minus.value
         print 'Fpm.minus.sigma =', self.Fpm.minus.sigma

      if self.Imean is None:
         print 'Imean =', self.Imean

      else:
         print 'Imean.value =', self.Imean.value
         print 'Imean.sigma =', self.Imean.sigma

      if self.Fmean is None:
         print 'Fmean =', self.Fmean

      else:
         print 'Fmean.value =', self.Fmean.value
         print 'Fmean.sigma =', self.Fmean.sigma

      print


class mtz_file(list):
   H = K = L = FREE = HM = None

   def _parse_header(self, lines, vb):
      if vb:
         print
         for line in lines:
            print line

         print

      header_dict = dict()
      for line in lines:
         key, sep, data = line.partition(' ')
         data_list = header_dict.get(key, None)
         if not data_list:
            data_list = list()
            header_dict[key] = data_list

         data_list.append(data)

      columns_dict = dict()
      for data in header_dict['COLUMN']:
         words = data.split()
         label = words[0]
         type = words[1]
         index = words[4]
         if type == 'H' and label in ('H', 'K', 'L'):
            if getattr(self, label, None):
               raise Exception()

            else:
               setattr(self, label, label)

         if type == 'I' and label.upper().find('FREE') >= 0:
            if not self.FREE:
               self.FREE = label

         columns = columns_dict.get(index, None)
         if not columns:
            columns = dict()
            columns_dict[index] = columns

         labs = columns.get(type, None)
         if not labs:
            labs = list()
            columns[type] = labs

         labs.append(label)

      if 'SYMINF' in header_dict:
         data = header_dict['SYMINF'][0].replace("'", '"')
         lpos = data.find('"') + 1
         rpos = data.rfind('"')
         if 0 < lpos and lpos < rpos:
            self.HM = data[lpos:rpos]

         else:
            self.HM = header_dict['SYMINF'][0].split()[4]

      if not (self.H and self.K and self.L and self.HM):
         raise Exception()

      ds_dict = dict()
      for index, columns in columns_dict.items():
         if vb:
            print columns

         ds_dict[index] = dataset(columns, self)

      for key in 'PROJECT', 'CRYSTAL', 'DATASET', 'DCELL', 'DWAVEL':
         if key in header_dict:
            for data in header_dict[key]:
               words = data.split()
               obj = ds_dict[words[0]]
               if key == 'DCELL':
                  value = list()
                  for word in words[1:]:
                     value.append(float(word))

                  setattr(obj, key, tuple(value))

               elif key == 'DWAVEL':
                  setattr(obj, key, float(words[1]))

               else:
                  setattr(obj, key, words[1])

      for index in sorted(ds_dict):
         ds = ds_dict[index]
         if ds.Ipm or ds.Fpm or ds.Imean or ds.Fmean:
            setattr(ds, 'ID', index)
            self.append(ds)

   def __init__(self, fname, vb=False):
      self.MTZ = os.path.abspath(fname)
      with open(fname, mode='rb') as istream:
         content = istream.read()
         n = struct.unpack("<i", content[4:8])[0] - 1
         h = content[4*n:]
         lines = list()
         for i in range(0,len(h),80):
            line = h[i:i+80]
            lines.append(line)

         self._parse_header(lines, vb)


if __name__ == '__main__':

   mf = mtz_file('/Applications/ccp4-6.5/examples/tutorial/data/hg_a_1to84_h3_scala2.mtz')
   for ds in mf:
      ds.prn()

   mf = mtz_file('/Applications/ccp4-6.5/examples/toxd/toxd.mtz')
   for ds in mf:
      ds.prn()

   print "----------------------------------------------------------"
   for ds in mf:
      print ds.to_JSON()
      print "----------------------------------------------------------"

