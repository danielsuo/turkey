import os
import re

with open('tmp.out', 'w') as w:
  for directory in os.walk('./out'):
    directory = directory[0]

    if directory[-3:] == 'out':
      continue

    params = directory.split('/')[-1].split('_')
    output_path = os.path.join(directory, 'task.out')

    with open(output_path, 'r') as f:
      for line in f:
        if line.find('real') > -1 or line.find('user') > -1 or line.find('sys') > -1:
          try:
            m = re.search('(\d+\.\d+)', line)
            result = float(m.group(1)) * 1000

            params.append(str(result))
          except AttributeError as e:
            print(e)
            continue

    w.write(','.join(params) + '\n')
