
import os, sys, re, shutil, time
import subprocess as SP
from pyrvapi import *


class pyrvapi_document(object):

    def __init__(self, wintitle, ccp4=None, workdir='report', html='index.html', xml='output.xml'):
        if not ccp4:
            ccp4 = os.environ['CCP4']

        if os.path.isabs(workdir):
            self.workdir = workdir

        else:
            self.workdir = os.path.join(os.getcwd(), workdir)

        if os.path.exists(self.workdir):
            shutil.rmtree(self.workdir)

        os.mkdir(self.workdir)
        self.html = html

        jsrview_fname = 'jsrview.exe' if sys.platform.startswith('win') else 'jsrview'
        self.jsrview_exe = os.path.join(ccp4, 'libexec', jsrview_fname)
        self.jsrview_lib = os.path.join(ccp4, 'share', 'jsrview')

        #// Document modes
        #define RVAPI_MODE_Silent  0x00100000
        #define RVAPI_MODE_Html    0x00000001
        #define RVAPI_MODE_Xmli2   0x00000002

        mode = RVAPI_MODE_Html | RVAPI_MODE_Xmli2

        #// Document layouts
        #define RVAPI_LAYOUT_Header   0x00000001
        #define RVAPI_LAYOUT_Toolbar  0x00000002
        #define RVAPI_LAYOUT_Tabs     0x00000004
        #define RVAPI_LAYOUT_Full     0x00000007

        layout = RVAPI_LAYOUT_Full

        docid = 'TestRun'
        rvapi_init_document(
            docid,             # const char * docId      // mandatory
            self.workdir,      # const char * outDir     // mandatory
            wintitle,          # const char * winTitle   // mandatory
            mode,              # const int    mode       // mandatory
            layout,            # const int    layout     // mandatory
            self.jsrview_lib,  # const char * jsUri      // needed
            None,              # const char * helpFName  // may be NULL
            self.html,         # const char * htmlFName  // may be NULL
            None,              # const char * taskFName  // may be NULL
            xml                # const char * xmli2FName // may be NULL
        )

    def write_fileinfo(self, ostream):
        print >>ostream, 'Qt browser:         ' + self.jsrview_exe
        print >>ostream, 'JavaScript library: ' + self.jsrview_lib
        print >>ostream, 'Working directory:  ' + self.workdir
        print >>ostream, 'Report directory:   ' + self.workdir

    def start_jsrview(self):
        jsrv_stdo = os.path.join(self.workdir, 'stdout.txt')
        jsrv_stde = os.path.join(self.workdir, 'stderr.txt')
        jsrv_html = os.path.join(self.workdir, self.html)
        with open(jsrv_stdo, 'w') as stdo, open(jsrv_stde, 'w') as stde:
            SP.Popen((self.jsrview_exe, jsrv_html), stdout=stdo, stderr=stde)

    def add_header(self, header):
        rvapi_add_header(header)


class Counter(object):
  cou = 0
  @classmethod
  def get_id(cls):
    cls.cou += 1
    return "%s_%02d" %(cls.__name__, cls.cou)

  def __str__(self):
    return self.id


class pyrvapi_tab(Counter):
    def __init__(self, name, open=True):
        self.id = self.get_id()
        rvapi_add_tab(self.id, name, open)


class pyrvapi_section(Counter):
    def __init__(self, t, title, irow, icol, nrow, ncol, open=True):
        self.id = self.get_id()
        rvapi_add_section(self.id, title, t.id, irow, icol, nrow, ncol, open)


class pyrvapi_g_widget(Counter):

    def __init__(self, s, irow, icol, nrow, ncol, vb=False):
        self.vb = vb
        self.id = self.get_id()
        s_id = s if type(s) is str else s.id
        rvapi_add_loggraph(self.id, s_id, irow, icol, nrow, ncol)


class pyrvapi_g_folder(Counter):

    def __init__(self, w, title='folder'):
        self.w = w
        self.id = self.get_id()
        rvapi_add_graph_data(self.id, self.w.id, title)


class pyrvapi_g_plot(Counter):

    def __init__(self, f, title='plot', xlab='x', ylab='y'):
        self.f = f
        self.id = self.get_id()
        rvapi_add_graph_plot(self.id, self.f.w.id, title, xlab, ylab)

    def set_default(self):
        rvapi_set_default_plot(self.id, self.f.w.id)

    def set_xlim(self, xmin, xmax):
        rvapi_set_plot_xrange(self.id, self.f.w.id, xmin, xmax)

    def set_ylim(self, ymin, ymax):
        rvapi_set_plot_yrange(self.id, self.f.w.id, ymin, ymax)

    def set_legend(self, location, placement):
        rvapi_set_plot_legend(self.id, self.f.w.id, location, placement)


def pyrvapi_g_line(p, x, y):
    rvapi_add_plot_line(p.id, p.f.id, p.f.w.id, x.id, y.id)


class pyrvapi_g_array(Counter):

    def __init__(self, p, name='array', header='Array'):
        self.p = p
        self.id = self.get_id()
        rvapi_add_graph_dataset(self.id, self.p.f.id, self.p.f.w.id, name, header)

    def reset(self):
        rvapi_reset_graph_dataset(self.id, self.p.f.id, self.p.f.w.id)


class pyrvapi_g_iarray(pyrvapi_g_array):

    def add(self, inp):
        rvapi_add_graph_int(self.id, self.p.f.id, self.p.f.w.id, int(inp))


class pyrvapi_g_darray(pyrvapi_g_array):

    def add(self, inp):
        rvapi_add_graph_real(self.id, self.p.f.id, self.p.f.w.id, float(inp), '%g')


class LogDataLine(list):

    def __init__(self, pattern=''):
        self.regex = re.compile(pattern)
        self.action_list = list()

    def add_action(self, *action_list):
        self.action_list.extend(action_list)

    def try_next(self, line, jobdict, verbose=False, pause=0):
        for next in self:
            data = next.regex.match(line)
            if data:
                items = data.groups()
                if pause:
                    time.sleep(pause)

                for action in next.action_list:
                    if verbose:
                        print str(action).split()[2], items

                    action(items, jobdict)

                return next

        return self


class LogDataParser(object):

    def __init__(self, starting_item, **kwargs):
        self.completed = LogDataLine()
        self.completed.append(starting_item)
        self.kwargs = kwargs
        self.jobdict = dict()

    def parse_line(self, line):
        self.completed = self.completed.try_next(line, self.jobdict, **self.kwargs)

    def parse_stream(self, istream, ostream=None):
        line = istream.readline()
        while line:
            if ostream:
                print >>ostream, line,
                ostream.flush()

            self.parse_line(line)
            line = istream.readline()


class MLP_parser(LogDataParser):

    def __init__(self, **kwargs):
        i = ' +(-?[0-9]+)'
        r = ' +(-?[0-9]+\.[0-9]*)'
        x = ' +\((-?[0-9]+\.[0-9]*)\)'
        b = '^'
        v = ' *\|'
        e = ' *$'
        crf_title = '^     --- Peaks of Rotation Function ---$'
        tf_title = '^  --- Translation function ---$'
        crf_head = '^ +\| +theta +phi +chi +Rf/sigma \|$'
        tf_head = '^ +RF +TF +TF/sig +TFcntrst +PFind +PF'
        tf_head += ' +PFmin +wRfac +Score +Cntrst +\(for\)$'
        coord_conv = '^ --- convert "molrep.crd" to "molrep.pdb" ---$'

        self.crf_startline = LogDataLine(crf_head)
        self.crf_dataline = LogDataLine(b + v + i + 4* r + v + e)
        self.crf_finishline = LogDataLine('^ \+-+\+$')
        self.tf_startline = LogDataLine(tf_head)
        self.tf_dataline = LogDataLine(b + 2* i + 8* r + x + e)
        self.tf_finishline = LogDataLine('^ \+-+\+$')
        self.success = LogDataLine(coord_conv)

        self.crf_startline.append(self.crf_dataline)
        self.crf_dataline.extend((self.crf_dataline, self.crf_finishline))
        self.crf_finishline.append(self.tf_startline)
        self.tf_startline.append(self.tf_dataline)
        self.tf_dataline.extend((self.tf_dataline, self.tf_finishline))
        self.tf_finishline.extend((self.tf_startline, self.success))

        LogDataParser.__init__(self, self.crf_startline, **kwargs)


class MLP_plot_rf(object):

    def __init__(self, parser, f):
        parser.crf_startline.add_action(self.start)
        parser.crf_dataline.add_action(self.add_data)
        parser.crf_finishline.add_action(self.finish)
        self.x_index = 0
        self.y_index = 4

        p = pyrvapi_g_plot(f, title='RF based Z-score', xlab='RF peak No', ylab='RF/sigRF')
        p.set_xlim(0, 44)
        p.set_ylim(-0.02, 0.6)
        self.x = pyrvapi_g_iarray(p, name='RF peak No', header='Rotation Function Peak Rank')
        self.y = pyrvapi_g_darray(p, name='RF/sigRF', header='RF based Z-score')
        pyrvapi_g_line(p, self.x, self.y)

    def start(self, items, jobdict):
        jobdict['np_rf'] = 0

    def add_data(self, items, jobdict):
        self.x.add(int(items[self.x_index]))
        self.y.add(float(items[self.y_index]))
        rvapi_flush()
        jobdict['np_rf'] += 1

    def finish(self, items, jobdict):
        pass


class MLP_plot_tf(object):

    def __init__(self, parser, f):
        parser.tf_startline.add_action(self.start)
        parser.tf_dataline.add_action(self.add_data)
        parser.tf_finishline.add_action(self.finish)
        self.x_index = 0
        self.cou = 0

        self.p = pyrvapi_g_plot(f, **self.p_kwargs)
        self.p.set_legend(RVAPI_LEGEND_LOC_SE, RVAPI_LEGEND_PLACE_Inside)

    def start(self, items, jobdict):
        self.cou += 1
        self.p.set_xlim(0, jobdict['np_rf'])

        y_kwargs = dict(
            name = self.y_fmt['name'] %self.cou,
            header = self.y_fmt['header'] %self.cou,
        )

        self.x = pyrvapi_g_iarray(self.p, **self.x_kwargs)
        self.y = pyrvapi_g_darray(self.p, **y_kwargs)
        pyrvapi_g_line(self.p, self.x, self.y)

    def add_data(self, items, jobdict):
        self.x.add(int(items[self.x_index]))
        self.y.add(float(items[self.y_index]))
        rvapi_flush()

    def finish(self, items, jobdict):
        pass

    x_kwargs = dict(
        name='RF peak No',
        header='Rotation Function Peak Rank'
    )


class MLP_plot_tfc(MLP_plot_tf):

    y_index = 8

    p_kwargs = dict(
        title='Correlation Coefficient',
        xlab='RF peak No',
        ylab='CC',
    )

    y_fmt = dict(
        name = 'TF CC for copy %d',
        header = 'TF Correlation Coefficient for copy %d',
    )


class MLP_plot_tfz(MLP_plot_tf):

    y_index = 2

    p_kwargs = dict(
        title='TF based Z-score',
        xlab='RF peak No',
        ylab='TF/sigTF',
    )

    y_fmt = dict(
        name = 'TF/sigTF for copy %d',
        header = 'TF based Z-score for copy %d',
    )


class MLP_show_output(object):

    def __init__(self, parser):
        parser.success.add_action(self.info)

    def info(self, items, jobdict):
        pass


class pyrvapi_logparse_molrep(MLP_parser):

    def __init__(self, s, **kwargs):
        MLP_parser.__init__(self, **kwargs)

        w = pyrvapi_g_widget(s, 0, 1, 1, 1, vb=True)

        f = pyrvapi_g_folder(w, title='Rotation Function')
        plot_rf = MLP_plot_rf(self, f)

        f = pyrvapi_g_folder(w, title='Translation Function')
        plot_tf = MLP_plot_tfc(self, f)
        plot_tf = MLP_plot_tfz(self, f)

        output = MLP_show_output(self)


def pyrvapi_logparse_molrep_test():
    import argparse
    parser = argparse.ArgumentParser(
       description='Test run of molrep log parser.'
    )
    parser.add_argument(
       'input',
       help='molrep log file to parse',
       type=file,
       metavar='<file>'
    )
    parser.add_argument(
       '-o', '--output',
       help='copy standard input to standard output',
       action='store_true'
    )
    parser.add_argument(
       '-j', '--jsrview',
       help='launch jsrview and display results',
       action='store_true'
    )
    parser.add_argument(
       '-v', '--verbose',
       help='print results of parsing for each recognised line',
       action='store_true'
    )
    parser.add_argument(
       '-p', '--pause',
       help='pause in seconds after each recognised line and before any associaded action action',
       default=0,
       type=float,
       metavar='<float>'
    )
    opt = parser.parse_args()

    d = pyrvapi_document('RVAPI Demo 1')
    if opt.verbose:
        d.write_fileinfo(sys.stdout)

    if opt.jsrview:
        d.start_jsrview()

    d.add_header('RVAPI Demo Page 1')
    t = pyrvapi_tab('Report')
    s = pyrvapi_section(t, 'Results', 0, 0, 1, 1)
    rvapi_flush()

    istream = opt.input
    ostream = sys.stdout if opt.output else None
    parser = pyrvapi_logparse_molrep(s.id, verbose=opt.verbose, pause=opt.pause)

    parser.parse_stream(istream, ostream)

#   line = istream.readline()
#   while line:
#       parser.parse_line(line)
#       line = istream.readline()


if __name__ == '__main__':
    pyrvapi_logparse_molrep_test()


