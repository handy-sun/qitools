"""Tests for image conversion."""

import pytest
from pathlib import Path
from PIL import Image
from qitools_cli.core.image_converter import convert_image, resize_image


def create_test_image(tmp_path, filename="test.png", size=(100, 100)):
    """Helper function to create a test image."""
    img = Image.new('RGB', size, color='red')
    filepath = tmp_path / filename
    img.save(filepath)
    return filepath


def test_convert_image_png_to_jpg(tmp_path):
    """Test PNG to JPG conversion."""
    # Create PNG image
    input_file = create_test_image(tmp_path, "test.png")
    output_file = tmp_path / "test.jpg"

    # Convert
    convert_image(str(input_file), str(output_file), "jpg")

    # Verify output exists and is JPG
    assert output_file.exists()
    img = Image.open(output_file)
    assert img.format == "JPEG"


def test_convert_image_jpg_to_png(tmp_path):
    """Test JPG to PNG conversion."""
    # Create JPG image
    input_file = create_test_image(tmp_path, "test.jpg")
    output_file = tmp_path / "test.png"

    # Convert
    convert_image(str(input_file), str(output_file), "png")

    # Verify output exists and is PNG
    assert output_file.exists()
    img = Image.open(output_file)
    assert img.format == "PNG"


def test_resize_image_width_only(tmp_path):
    """Test image resize with width only."""
    # Create image
    input_file = create_test_image(tmp_path, "test.png", size=(200, 100))
    output_file = tmp_path / "resized.png"

    # Resize with keep_aspect to maintain aspect ratio
    resize_image(str(input_file), str(output_file), width=100, keep_aspect=True)

    # Verify size
    img = Image.open(output_file)
    assert img.width == 100
    assert img.height == 50  # Should maintain aspect ratio


def test_resize_image_height_only(tmp_path):
    """Test image resize with height only."""
    # Create image
    input_file = create_test_image(tmp_path, "test.png", size=(200, 100))
    output_file = tmp_path / "resized.png"

    # Resize with keep_aspect to maintain aspect ratio
    resize_image(str(input_file), str(output_file), height=50, keep_aspect=True)

    # Verify size
    img = Image.open(output_file)
    assert img.width == 100
    assert img.height == 50


def test_resize_image_both_dimensions(tmp_path):
    """Test image resize with both dimensions."""
    # Create image
    input_file = create_test_image(tmp_path, "test.png", size=(200, 100))
    output_file = tmp_path / "resized.png"

    # Resize without keeping aspect ratio
    resize_image(str(input_file), str(output_file), width=100, height=100)

    # Verify size
    img = Image.open(output_file)
    assert img.width == 100
    assert img.height == 100


def test_resize_image_keep_aspect(tmp_path):
    """Test image resize with aspect ratio preservation."""
    # Create image
    input_file = create_test_image(tmp_path, "test.png", size=(200, 100))
    output_file = tmp_path / "resized.png"

    # Resize with keep aspect
    resize_image(str(input_file), str(output_file), width=100, height=100, keep_aspect=True)

    # Verify size - should fit within 100x100
    img = Image.open(output_file)
    assert img.width <= 100
    assert img.height <= 100


def test_resize_image_no_dimensions(tmp_path):
    """Test error when no dimensions specified."""
    input_file = create_test_image(tmp_path, "test.png")

    with pytest.raises(ValueError):
        resize_image(str(input_file), "output.png")