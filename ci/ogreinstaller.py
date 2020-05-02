import requests
from tempfile import NamedTemporaryFile
from zipfile import ZipFile

def fetch(url: str, path: str):
    with requests.get(url, stream=True) as stream, NamedTemporaryFile() as file:

        # Download
        stream.raise_for_status()
        for chunk in stream.iter_content(chunk_size=8192):
            file.write(chunk)
        file.flush()

        # extract zip file
        with ZipFile(path, "r") as zip:
            zip.extractall(path)

windows_url = "https://bintray.com/ogrecave/ogre/download_file?file_path=ogre-sdk-v1.12.6-vc15-x64.zip"

fetch(windows_url, "libs/Ogre")
