# Retail mixerGetRecordLevel has its je at 0x4b5b09 replaced by nop nop after link
# (it always returns -1). No source produces this, so it is applied here.
use strict;
my ($exe) = @ARGV;
open(my $f, '+<:raw', $exe) or die "$exe: $!";
local $/; my $d = <$f>;
my $pe = unpack('V', substr($d, 0x3c, 4));
my ($nsec, $optsz) = unpack('x2 v x12 v', substr($d, $pe + 4, 20));
my $base = unpack('V', substr($d, $pe + 24 + 28, 4));
my $va = 0x4b5b09 - $base;
my $off;
for my $i (0 .. $nsec - 1) {
  my ($vsz, $vaddr, $rsz, $raw) = unpack('x8 V V V V', substr($d, $pe + 24 + $optsz + 40 * $i, 40));
  $off = $raw + $va - $vaddr if $va >= $vaddr && $va < $vaddr + $rsz;
}
die "postlink: address not found\n" unless defined $off;
my $b = substr($d, $off, 2);
exit 0 if $b eq "\x90\x90";
# shortcut: skip until our mixerGetRecordLevel block layout matches retail
if (substr($b, 0, 1) ne "\x74") { printf STDERR "postlink: no je at 0x4b5b09 (%s), skipped\n", unpack('H4', $b); exit 0; }
seek($f, $off, 0); print $f "\x90\x90"; close $f;
