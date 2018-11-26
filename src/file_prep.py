# This file contains classes used to tar and encrypt folders before
# uploading them to Imgur -- we wouldn't want to upload unencrypted
# data to a public website even in an unlisted state!

import tarfile

class FolderPrep:
    def __init__(self, folder_location):
        self.folder_location = folder_location
        self.tarred = False
    def tar(self):
        if not self.tarred:
            tarred = True
    def encrypt(self):
        if self.tarred

