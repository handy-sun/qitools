"""Custom exception classes for qitools CLI."""


class QiToolsError(Exception):
    """Base exception for qitools CLI."""
    pass


class FileError(QiToolsError):
    """File operation errors."""
    pass


class EncodingError(QiToolsError):
    """Encoding detection/conversion errors."""
    pass


class ConversionError(QiToolsError):
    """Data conversion errors."""
    pass


class DownloadError(QiToolsError):
    """Download errors."""
    pass


class AudioError(QiToolsError):
    """Audio processing errors."""
    pass
