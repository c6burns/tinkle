using System;
using System.Text;
using System.Threading;
using System.Runtime.InteropServices;
using System.Security;
using System.Diagnostics;
using UnityEngine;

[SuppressUnmanagedCodeSecurity]
public unsafe class TinkleNative : MonoBehaviour
{
    [StructLayout(LayoutKind.Explicit, CharSet = CharSet.Ansi, Size = 256)]
    public struct BacktraceFrame
    {
        [FieldOffset(0)] public int line;
        [FieldOffset(4)] public fixed byte func[128 - 4];
        [FieldOffset(128)] public fixed byte file[128];
    }

    [StructLayout(LayoutKind.Explicit, CharSet = CharSet.Ansi, Size = (256 * 128) + 8)]
    public struct Backtrace
    {
        [FieldOffset(0)] int _numFrames;
        [FieldOffset(4)] int _maxFrames;
        [FieldOffset(8)] BacktraceFrame _frameList;

        public Backtrace(int max = 128)
        {
            _frameList = default;
            _numFrames = 0;
            _maxFrames = max;
            if (_maxFrames > 128) _maxFrames = 128;
        }

        public void Clear()
        {
            _numFrames = 0;
        }

        public bool Add(string file, string func, int line)
        {
            if (_numFrames >= _maxFrames) return false;

            fixed (char *pfile = file)
            fixed (char *pfunc = func)
            fixed (BacktraceFrame *frameBase = &_frameList)
            {
                BacktraceFrame *frame = frameBase + _numFrames++;
                UTF8Encoding.UTF8.GetBytes(pfunc, func.Length, frame->func, 128 - 4 - 1);
                UTF8Encoding.UTF8.GetBytes(pfile, file.Length, frame->file, 128 - 1);
                frame->line = line;
            }

            return true;
        }
    }


    private byte[] _snaco;


    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate int OnBacktraceRequest(Backtrace* bt);


    private static int BacktraceRequestHandler(Backtrace* bt)
    {
        bt->Clear();
        StackTrace st = new StackTrace(1, true);
        StackFrame[] stFrames = st.GetFrames();
        foreach (StackFrame frame in stFrames)
        {
            string file = frame.GetFileName();
            string func = string.Empty;
            int line = frame.GetFileLineNumber();

            System.Reflection.MethodBase mb = frame.GetMethod();
            if (mb != null)
            {
                if (mb.DeclaringType != null)
                {
                    func = mb.DeclaringType.FullName + "." + mb.Name;
                } else
                {
                    func = mb.Name;
                }
            }

            bt->Add(file, func, line);
        }
        return 0;
    }

    private OnBacktraceRequest _backtraceRequestHandler = new OnBacktraceRequest(BacktraceRequestHandler);


    [DllImport("tnkl", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    private static extern int RegisterBacktraceCallback(OnBacktraceRequest callback);

    void Start()
    {
        RegisterBacktraceCallback(_backtraceRequestHandler);
    }

    void Update()
    {
        _snaco = new byte[1024];
    }
}
