import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.address.*;
import java.io.*;
// Lst.java OUT ADDR: list every instruction of the (PGO-split) function containing ADDR, by address
public class Lst extends GhidraScript {
  public void run() throws Exception {
    String[] a = getScriptArgs();
    Function f = getFunctionContaining(toAddr(Long.parseLong(a[1], 16)));
    PrintWriter w = new PrintWriter(a[0]);
    InstructionIterator it = currentProgram.getListing().getInstructions(f.getBody(), true);
    while (it.hasNext()) { Instruction i = it.next(); w.println(i.getAddress() + " " + i); }
    w.close();
  }
}
