import argparse
import urllib.request
import shutil
import os

parser = argparse.ArgumentParser()
parser.add_argument('url')
parser.add_argument('filename')

args = parser.parse_args()

dn = os.path.dirname(args.filename)
if len(dn) > 0:
    os.makedirs(os.path.dirname(args.filename), exist_ok=True)

with urllib.request.urlopen(args.url) as response, open(args.filename, 'wb') as out_file:
    shutil.copyfileobj(response, out_file)
