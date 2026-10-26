cp /mnt/d/vs2002x/sr1.py /home/nawaf/cod2/test/dev/lab/m1/sr1.py
cd /home/nawaf/cod2/test/dev/claude_client
bash /mnt/d/vs2002x/rps.sh m1_s1 oc/snd.cpp /home/nawaf/cod2/test/dev/lab/m1/sr1.py
bash /mnt/d/vs2002x/fx.sh m1_s1 '?SND_RestoreStreamChannel@@' 3000 > /home/nawaf/cod2/test/dev/lab/m1/s1.diff
