using System.Text;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public class TinkleNative : MonoBehaviour
{
    private byte[] _snaco;

    [DllImport("tnkl", CharSet = CharSet.Ansi)]public static extern void tinkletytinkerz(StringBuilder str, ulong sz);

    void Start()
    {
    }

    void Update()
    {
        _snaco = new byte[1024];
    }

    private void OnGUI()
    {
        if (GUILayout.Button("Retreieve Markers"))
        {
            StringBuilder bigStr = new StringBuilder(9857893, 9857893);
            TinkleNative.tinkletytinkerz(bigStr, 9857893);
            Debug.Log(bigStr.ToString());
        }
        
    }
}
