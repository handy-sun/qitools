"""Tests for download functionality."""

import pytest
from pathlib import Path
from unittest.mock import patch, MagicMock
from qitools_cli.core.downloader import download_file


def test_download_file_success(tmp_path):
    """Test successful file download."""
    # Mock requests.get
    mock_response = MagicMock()
    mock_response.iter_content.return_value = [b"file content"]
    mock_response.raise_for_status.return_value = None

    with patch('qitools_cli.core.downloader.requests.get', return_value=mock_response):
        output_file = tmp_path / "downloaded.txt"
        download_file("https://example.com/file.txt", str(output_file))

        # Verify file was created
        assert output_file.exists()
        assert output_file.read_text() == "file content"


def test_download_file_creates_directory(tmp_path):
    """Test that download creates parent directories."""
    # Mock requests.get
    mock_response = MagicMock()
    mock_response.iter_content.return_value = [b"content"]
    mock_response.raise_for_status.return_value = None

    with patch('qitools_cli.core.downloader.requests.get', return_value=mock_response):
        output_file = tmp_path / "subdir" / "file.txt"
        download_file("https://example.com/file.txt", str(output_file))

        # Verify file was created in subdirectory
        assert output_file.exists()


def test_download_file_chunked(tmp_path):
    """Test that download writes in chunks."""
    # Mock requests.get with multiple chunks
    mock_response = MagicMock()
    mock_response.iter_content.return_value = [b"chunk1", b"chunk2", b"chunk3"]
    mock_response.raise_for_status.return_value = None

    with patch('qitools_cli.core.downloader.requests.get', return_value=mock_response):
        output_file = tmp_path / "chunked.txt"
        download_file("https://example.com/file.txt", str(output_file), chunk_size=1024)

        # Verify all chunks were written
        assert output_file.read_text() == "chunk1chunk2chunk3"


def test_download_file_http_error(tmp_path):
    """Test download with HTTP error."""
    # Mock requests.get to raise error
    mock_response = MagicMock()
    mock_response.raise_for_status.side_effect = Exception("404 Not Found")

    with patch('qitools_cli.core.downloader.requests.get', return_value=mock_response):
        output_file = tmp_path / "error.txt"

        with pytest.raises(Exception):
            download_file("https://example.com/notfound.txt", str(output_file))


def test_download_file_network_error(tmp_path):
    """Test download with network error."""
    # Mock requests.get to raise network error
    with patch('qitools_cli.core.downloader.requests.get', side_effect=Exception("Network error")):
        output_file = tmp_path / "network_error.txt"

        with pytest.raises(Exception):
            download_file("https://example.com/file.txt", str(output_file))