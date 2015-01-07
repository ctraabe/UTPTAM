import os

# This function executes PTAM for each value of a parameter according to the
# specified interval and range.
def ptam_over_param_range( group, param, minv, maxv, interval, exponent, batch ):
  for paramv in range(minv, maxv + interval, interval):
    cmd = 'cp settings_template.cfg settings.cfg'
    os.system(cmd)

    myfile = open(r"settings.cfg","a")
    myfile.write("\n" + group + "." + param + "=" + str(paramv * pow(10.0, \
      exponent)))
    myfile.close()

    cmd = './UTPTAM'
    os.system(cmd)

    cmd = 'mv coordinates.log batch' + str(batch) + '/' + group + '_' + param \
      + '_' + str(paramv) + 'e' + str(exponent) + '.txt'
    os.system(cmd)
  return


# Go to the PTAM directory.
os.chdir('..')

for batch in range(1, 10, 1):
  cmd = 'mkdir -p batch' + str(batch)
  os.system(cmd)

  # Run PTAM over each of the following paramter ranges:
  ptam_over_param_range('Map', 'PatchSSDThreshold', 50, 200, 50, 2, batch)