"""Tests for color conversion."""

import pytest
from qitools_cli.core.color_converter import parse_color, format_color


def test_parse_color_hex():
    """Test hex color parsing."""
    # Test with #
    r, g, b = parse_color(hex_color="#FF0000")
    assert r == 255
    assert g == 0
    assert b == 0

    # Test without #
    r, g, b = parse_color(hex_color="00FF00")
    assert r == 0
    assert g == 255
    assert b == 0

    # Test 3-digit hex
    r, g, b = parse_color(hex_color="F00")
    assert r == 255
    assert g == 0
    assert b == 0


def test_parse_color_rgb():
    """Test RGB color parsing."""
    r, g, b = parse_color(rgb_color="255,0,0")
    assert r == 255
    assert g == 0
    assert b == 0

    r, g, b = parse_color(rgb_color="0,255,0")
    assert r == 0
    assert g == 255
    assert b == 0


def test_parse_color_decimal():
    """Test decimal color parsing."""
    # 16711680 = 0xFF0000
    r, g, b = parse_color(dec_color="16711680")
    assert r == 255
    assert g == 0
    assert b == 0

    # 65280 = 0x00FF00
    r, g, b = parse_color(dec_color="65280")
    assert r == 0
    assert g == 255
    assert b == 0


def test_parse_color_gl():
    """Test GL color parsing."""
    r, g, b = parse_color(gl_color="1.0,0.0,0.0")
    assert r == 255
    assert g == 0
    assert b == 0

    r, g, b = parse_color(gl_color="0.0,1.0,0.0")
    assert r == 0
    assert g == 255
    assert b == 0


def test_parse_color_no_input():
    """Test error when no input provided."""
    with pytest.raises(ValueError):
        parse_color()


def test_parse_color_invalid_rgb():
    """Test error with invalid RGB format."""
    with pytest.raises(ValueError):
        parse_color(rgb_color="255,0")  # Missing blue component


def test_parse_color_invalid_gl():
    """Test error with invalid GL format."""
    with pytest.raises(ValueError):
        parse_color(gl_color="1.0,0.0")  # Missing blue component


def test_format_color_hex():
    """Test hex color formatting."""
    result = format_color(255, 0, 0, "hex")
    assert result == "#FF0000"

    result = format_color(0, 255, 0, "hex")
    assert result == "#00FF00"


def test_format_color_rgb():
    """Test RGB color formatting."""
    result = format_color(255, 0, 0, "rgb")
    assert result == "255,0,0"

    result = format_color(0, 255, 0, "rgb")
    assert result == "0,255,0"


def test_format_color_decimal():
    """Test decimal color formatting."""
    result = format_color(255, 0, 0, "dec")
    assert result == "16711680"

    result = format_color(0, 255, 0, "dec")
    assert result == "65280"


def test_format_color_gl():
    """Test GL color formatting."""
    result = format_color(255, 0, 0, "gl")
    assert result == "1.000,0.000,0.000"

    result = format_color(0, 255, 0, "gl")
    assert result == "0.000,1.000,0.000"


def test_format_color_all():
    """Test all formats output."""
    result = format_color(255, 0, 0, "all")
    assert isinstance(result, list)
    assert len(result) == 4

    # Check each format is present
    formats = [row[0] for row in result]
    assert "Hex" in formats
    assert "RGB" in formats
    assert "Decimal" in formats
    assert "GL" in formats


def test_format_color_invalid_format():
    """Test error with invalid format."""
    with pytest.raises(ValueError):
        format_color(255, 0, 0, "invalid")