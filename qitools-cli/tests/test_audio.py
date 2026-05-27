"""Tests for audio processing."""

import pytest
from pathlib import Path
from unittest.mock import patch, MagicMock
from qitools_cli.core.audio_processor import get_audio_metadata, set_audio_metadata, extract_cover


def test_get_audio_metadata(tmp_path):
    """Test getting audio metadata."""
    # Mock MP3 file with metadata
    mock_audio = MagicMock()
    mock_audio.info.length = 180.5
    mock_audio.info.sample_rate = 44100
    mock_audio.info.channels = 2
    mock_audio.info.bitrate = 320000
    mock_audio.tags = {
        "TIT2": "Test Song",
        "TPE1": "Test Artist",
        "TALB": "Test Album"
    }

    with patch('qitools_cli.core.audio_processor.MP3', return_value=mock_audio):
        meta = get_audio_metadata("test.mp3")

        assert meta["Duration"] == "180.50s"
        assert meta["Sample Rate"] == "44100 Hz"
        assert meta["Channels"] == "2"
        assert meta["Bitrate"] == "320 kbps"
        assert meta["Title"] == "Test Song"
        assert meta["Artist"] == "Test Artist"
        assert meta["Album"] == "Test Album"


def test_get_audio_metadata_no_tags(tmp_path):
    """Test getting audio metadata without tags."""
    # Mock MP3 file without tags
    mock_audio = MagicMock()
    mock_audio.info.length = 60.0
    mock_audio.info.sample_rate = 44100
    mock_audio.info.channels = 1
    mock_audio.info.bitrate = 128000
    mock_audio.tags = None

    with patch('qitools_cli.core.audio_processor.MP3', return_value=mock_audio):
        meta = get_audio_metadata("test.mp3")

        assert meta["Duration"] == "60.00s"
        assert "Title" not in meta
        assert "Artist" not in meta


def test_set_audio_metadata(tmp_path):
    """Test setting audio metadata."""
    # Mock MP3 file
    mock_audio = MagicMock()
    mock_audio.tags = MagicMock()

    with patch('qitools_cli.core.audio_processor.MP3', return_value=mock_audio):
        set_audio_metadata("test.mp3", title="New Title", artist="New Artist", album="New Album")

        # Verify tags were added
        mock_audio.tags.add.assert_called()
        mock_audio.save.assert_called_once()


def test_set_audio_metadata_no_tags(tmp_path):
    """Test setting audio metadata when no tags exist."""
    # Mock MP3 file without tags
    mock_audio = MagicMock()
    mock_audio.tags = None

    # Mock add_tags to set tags to a MagicMock
    def mock_add_tags():
        mock_audio.tags = MagicMock()

    mock_audio.add_tags.side_effect = mock_add_tags

    with patch('qitools_cli.core.audio_processor.MP3', return_value=mock_audio):
        set_audio_metadata("test.mp3", title="New Title")

        # Verify tags were created
        mock_audio.add_tags.assert_called_once()
        mock_audio.tags.add.assert_called()
        mock_audio.save.assert_called()


def test_extract_cover_success(tmp_path):
    """Test successful cover extraction."""
    # Mock MP3 file with cover
    mock_audio = MagicMock()
    mock_apic = MagicMock()
    mock_apic.data = b"fake image data"
    mock_audio.tags = {"APIC:": mock_apic}

    with patch('qitools_cli.core.audio_processor.MP3', return_value=mock_audio):
        output_file = tmp_path / "cover.jpg"
        extract_cover("test.mp3", str(output_file))

        # Verify cover was extracted
        assert output_file.exists()
        assert output_file.read_bytes() == b"fake image data"


def test_extract_cover_no_tags(tmp_path):
    """Test cover extraction when no tags exist."""
    # Mock MP3 file without tags
    mock_audio = MagicMock()
    mock_audio.tags = None

    with patch('qitools_cli.core.audio_processor.MP3', return_value=mock_audio):
        with pytest.raises(ValueError):
            extract_cover("test.mp3", "cover.jpg")


def test_extract_cover_no_cover(tmp_path):
    """Test cover extraction when no cover art exists."""
    # Mock MP3 file without cover
    mock_audio = MagicMock()
    mock_audio.tags = {"TIT2": "Test"}  # No APIC frame

    with patch('qitools_cli.core.audio_processor.MP3', return_value=mock_audio):
        with pytest.raises(ValueError):
            extract_cover("test.mp3", "cover.jpg")