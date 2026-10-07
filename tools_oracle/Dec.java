import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.address.*;
import java.io.*;
public class Dec extends GhidraScript {
  public void run() throws Exception {
    String[] a = getScriptArgs();
    String outdir = a[0];
    DecompInterface d = new DecompInterface();
    d.openProgram(currentProgram);
    for (int i = 1; i < a.length; i++) {
      Address ad = toAddr(Long.parseLong(a[i], 16));
      Function f = getFunctionContaining(ad);
      if (f == null) { println("nofn " + a[i]); continue; }
      DecompileResults r = d.decompileFunction(f, 120, monitor);
      String s = r.getDecompiledFunction() != null ? r.getDecompiledFunction().getC() : "FAIL";
      PrintWriter w = new PrintWriter(outdir + "/" + f.getEntryPoint() + ".c");
      w.println("// from " + a[i]); w.print(s); w.close();
      println("ok " + a[i] + " -> " + f.getEntryPoint());
    }
  }
}
