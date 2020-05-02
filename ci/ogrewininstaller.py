import os
from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile
zipurl = "https://bintray.com/ogrecave/ogre/download_file?file_path=ogre-sdk-v1.12.6-vc15-x64.zip"
with urlopen(zipurl) as zipresp:
    with ZipFile(BytesIO(zipresp.read())) as zfile:
        zfile.extractall(os.path.join(os.getcwd(), "libs", "Ogre"))
