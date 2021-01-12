from TrackedColorBlob import TrackedColorBlob


class TrackedObject:
    def getColorBlobs(self):
        color_blobs = []
        color_blobs.append(TrackedColorBlob())
        color_blobs.append(TrackedColorBlob())
        return color_blobs