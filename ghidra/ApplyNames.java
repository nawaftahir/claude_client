// Apply names from a TSV (hexaddr, name, source) to functions, creating missing ones.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import java.nio.file.*;

public class ApplyNames extends GhidraScript {
    @Override
    public void run() throws Exception {
        int named = 0, created = 0;
        for (String line : Files.readAllLines(Paths.get(getScriptArgs()[0]))) {
            String[] f = line.split("\t");
            Address a = toAddr(Long.parseLong(f[0], 16));
            Function fn = getFunctionAt(a);
            if (fn == null) {
                disassemble(a);
                fn = createFunction(a, null);
                if (fn == null)
                    continue;
                created++;
            }
            try {
                fn.setName(f[1].replace("::", "__"), SourceType.USER_DEFINED);
                named++;
            } catch (Exception e) {
                // duplicate name: keep the address-suffixed variant
                fn.setName(f[1].replace("::", "__") + "_" + f[0], SourceType.USER_DEFINED);
                named++;
            }
        }
        println("named " + named + ", created " + created);
    }
}
