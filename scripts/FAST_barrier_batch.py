import os


# Go to the PTAM directory.
os.chdir('..')


minv = 15
maxv = 35
interval = 5
for batch in range(1, 10, 1):
  cmd = 'mkdir -p FASTbatch' + str(batch)
  os.system(cmd)
  for Barrier0 in range(minv, maxv + interval, interval):
    for Barrier1 in range(minv, maxv + interval, interval):
      for Barrier2 in range(minv, maxv + interval, interval):
        for Barrier3 in range(minv, maxv + interval, interval):
          cmd = 'cp settings_template.cfg settings.cfg'
          os.system(cmd)

          myfile = open(r"settings.cfg","a")
          myfile.write("\n" + "FAST.Barrier0=" + str(Barrier0))
          myfile.write("\n" + "FAST.Barrier1=" + str(Barrier1))
          myfile.write("\n" + "FAST.Barrier2=" + str(Barrier2))
          myfile.write("\n" + "FAST.Barrier3=" + str(Barrier3))
          myfile.close()

          cmd = './UTPTAM'
          os.system(cmd)

          cmd = 'mv coordinates.log FASTbatch' + str(batch) + '/FAST' \
            + '_Barrier0_' + str(Barrier0) \
            + '_Barrier1_' + str(Barrier1) \
            + '_Barrier2_' + str(Barrier2) \
            + '_Barrier3_' + str(Barrier3) \
            + '.txt'
          os.system(cmd)
