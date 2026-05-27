"""HTTP download logic - no UI dependencies."""

import requests
from pathlib import Path


def download_file(url: str, output_file: str, chunk_size: int = 8192) -> None:
    """Download a file from URL.

    Args:
        url: URL to download
        output_file: Path to save the file
        chunk_size: Download chunk size in bytes
    """
    response = requests.get(url, stream=True)
    response.raise_for_status()

    Path(output_file).parent.mkdir(parents=True, exist_ok=True)

    with open(output_file, 'wb') as f:
        for chunk in response.iter_content(chunk_size=chunk_size):
            f.write(chunk)
