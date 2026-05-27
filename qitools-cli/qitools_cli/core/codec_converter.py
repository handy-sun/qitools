"""Encoding detection and conversion logic - no UI dependencies."""

from pathlib import Path
from charset_normalizer import from_bytes


def detect_encoding(file_path: str) -> str:
    """Detect file encoding using charset-normalizer.

    Args:
        file_path: Path to the file

    Returns:
        Detected encoding name
    """
    raw_data = Path(file_path).read_bytes()
    result = from_bytes(raw_data).best()
    if result is None:
        raise ValueError("Could not detect encoding")
    return result.encoding


def convert_encoding(
    file_path: str,
    from_encoding: str | None,
    to_encoding: str
) -> str:
    """Convert file encoding.

    Args:
        file_path: Path to the file
        from_encoding: Source encoding (auto-detect if None)
        to_encoding: Target encoding

    Returns:
        Converted content as string
    """
    raw_data = Path(file_path).read_bytes()

    # Auto-detect if not specified
    if from_encoding is None:
        result = from_bytes(raw_data).best()
        if result is None:
            raise ValueError("Could not detect encoding")
        from_encoding = result.encoding

    # Decode and re-encode
    content = raw_data.decode(from_encoding)
    return content
