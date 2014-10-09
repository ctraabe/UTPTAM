import os


# Go to the PTAM directory.
os.chdir('..')


minv = 5
maxv = 25
interval = 5
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

        cmd = 'mv coordinates.log coord' \
          + '_Barrier0_' + str(Barrier0) \
          + '_Barrier1_' + str(Barrier1) \
          + '_Barrier2_' + str(Barrier2) \
          + '_Barrier3_' + str(Barrier3) \
          + '.txt'
        os.system(cmd)
