// Decompile every function to <outdir>/<addr>_<name>.c (one packet per function).
import ghidra.app.decompiler.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import java.nio.file.*;

public class DecompileAll extends GhidraScript {
    @Override
    public void run() throws Exception {
        Path out = Paths.get(getScriptArgs()[0]);
        Files.createDirectories(out);
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        int n = 0;
        for (Function fn : currentProgram.getFunctionManager().getFunctions(true)) {
            DecompileResults r = di.decompileFunction(fn, 60, monitor);
            if (r == null || r.getDecompiledFunction() == null)
                continue;
            String s = fn.getName().replaceAll("[^A-Za-z0-9_]", "_");
            String name = String.format("%08x_%s.c", fn.getEntryPoint().getOffset(), s.length() > 80 ? s.substring(0, 80) : s);
            Files.writeString(out.resolve(name), r.getDecompiledFunction().getC());
            n++;
        }
        di.dispose();
        println("decompiled " + n);
    }
}
