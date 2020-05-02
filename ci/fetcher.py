from os.path import join
from tempfile import NamedTemporaryFile
from zipfile import BadZipFile, LargeZipFile, ZipFile, is_zipfile

from requests import HTTPError
from requests import get as requests_get


class Fetcher():
    """Fetches and extracts package using specified url."""

    def fetch(self, url: str, path: str):
        with requests_get(url, stream=True) as stream, NamedTemporaryFile() as file:
            self.__download(stream, file)
            self.__extract(file.name, path)

    @staticmethod
    def __download(stream, file):
        stream.raise_for_status()
        for chunk in stream.iter_content(chunk_size=8192):
            file.write(chunk)
        file.flush()


    @staticmethod
    def __extract(path: str, extract_to: str):
        # check zip file
        if not is_zipfile(path):
            raise FetcherException("Not a zip file.")

        # extract zip file
        try:
            with ZipFile(path, "r") as zip:
                zip.extractall(extract_to)
        except FileNotFoundError as ex:
            raise FetcherException("Unable to extract file. File not found.")
        except BadZipFile as ex:
            raise FetcherException("Unable to extract file. Bad zip file.")
        except LargeZipFile as ex:
            raise FetcherException("Unable to extract file. Zip file is too large.")
