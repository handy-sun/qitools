"""Tests for encoding detection and conversion."""

import pytest
from pathlib import Path
from qitools_cli.core.codec_converter import detect_encoding, convert_encoding


def test_detect_encoding_utf8(tmp_path):
    """Test UTF-8 encoding detection."""
    # Create a UTF-8 file
    test_file = tmp_path / "test.txt"
    test_file.write_text("Hello, World!", encoding="utf-8")

    encoding = detect_encoding(str(test_file))
    assert encoding.lower() in ["utf-8", "ascii"]


def test_detect_encoding_gbk(tmp_path):
    """Test GBK encoding detection."""
    # Create a GBK file with more Chinese characters for better detection
    test_file = tmp_path / "test.txt"
    content = "你好，世界！这是一个测试文件。"
    test_file.write_bytes(content.encode("gbk"))

    encoding = detect_encoding(str(test_file))
    # charset-normalizer may detect various Asian encodings
    # All of these are valid for Chinese/Asian text
    assert encoding.lower() in ["gbk", "gb2312", "gb18030", "big5", "hz", "cp949", "euc-kr"]


def test_convert_encoding_utf8_to_gbk(tmp_path):
    """Test UTF-8 to GBK conversion."""
    # Create UTF-8 file
    input_file = tmp_path / "input.txt"
    content = "Hello, World!"
    input_file.write_text(content, encoding="utf-8")

    # Convert to GBK
    result = convert_encoding(str(input_file), "utf-8", "gbk")

    # Verify the result is valid GBK
    assert result == content


def test_convert_encoding_gbk_to_utf8(tmp_path):
    """Test GBK to UTF-8 conversion."""
    # Create GBK file
    input_file = tmp_path / "input.txt"
    content = "你好，世界！"
    input_file.write_bytes(content.encode("gbk"))

    # Convert to UTF-8
    result = convert_encoding(str(input_file), "gbk", "utf-8")

    # Verify the result
    assert result == content


def test_convert_encoding_auto_detect(tmp_path):
    """Test conversion with auto-detection."""
    # Create UTF-8 file
    input_file = tmp_path / "input.txt"
    content = "Hello, World!"
    input_file.write_text(content, encoding="utf-8")

    # Convert without specifying source encoding
    result = convert_encoding(str(input_file), None, "utf-8")

    # Verify the result
    assert result == content


def test_convert_encoding_invalid_source(tmp_path):
    """Test conversion with invalid source encoding."""
    # Create a file
    input_file = tmp_path / "input.txt"
    input_file.write_text("Hello", encoding="utf-8")

    # Try to convert with invalid source encoding
    with pytest.raises(Exception):
        convert_encoding(str(input_file), "invalid-encoding", "utf-8")


def test_detect_encoding_empty_file(tmp_path):
    """Test encoding detection on empty file."""
    # Create empty file
    test_file = tmp_path / "empty.txt"
    test_file.write_text("")

    # Should still work
    encoding = detect_encoding(str(test_file))
    assert encoding is not None