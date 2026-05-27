"""Tests for C++ code generation."""

from qitools_cli.core.property_generator import generate_property_code


def test_generate_property_basic():
    """Test basic getter/setter generation."""
    result = generate_property_code("int", "count")
    assert "int count() const;" in result
    assert "void setCount(const int& count);" in result


def test_generate_property_with_notify():
    """Test getter/setter with notify signal."""
    result = generate_property_code("int", "count", notify=True)
    assert "int count() const;" in result
    assert "void setCount(const int& count);" in result
    assert "void countChanged();" in result
    assert "signals:" in result


def test_generate_property_with_prefix():
    """Test getter/setter with custom prefix."""
    result = generate_property_code("int", "count", prefix="m_")
    assert "int count() const;" in result
    assert "void setCount(const int& count);" in result


def test_generate_property_inline():
    """Test inline code generation."""
    result = generate_property_code("int", "count", inline=True)
    assert "int count() const { return m_count; }" in result
    assert "void setCount(const int& count) {" in result
    assert "if (m_count != count) {" in result
    assert "m_count = count;" in result


def test_generate_property_inline_with_notify():
    """Test inline code generation with notify signal."""
    result = generate_property_code("int", "count", notify=True, inline=True)
    assert "int count() const { return m_count; }" in result
    assert "void setCount(const int& count) {" in result
    assert "emit countChanged();" in result
    assert "void countChanged();" in result


def test_generate_property_different_types():
    """Test generation with different property types."""
    # Test with QString
    result = generate_property_code("QString", "name")
    assert "QString name() const;" in result
    assert "void setName(const QString& name);" in result

    # Test with bool
    result = generate_property_code("bool", "visible")
    assert "bool visible() const;" in result
    assert "void setVisible(const bool& visible);" in result

    # Test with custom type
    result = generate_property_code("MyClass", "data")
    assert "MyClass data() const;" in result
    assert "void setData(const MyClass& data);" in result


def test_generate_property_capitalization():
    """Test proper capitalization of property names."""
    # Test with lowercase name
    result = generate_property_code("int", "count")
    assert "setCount" in result

    # Test with camelCase name
    result = generate_property_code("int", "totalCount")
    assert "setTotalCount" in result

    # Test with single letter name
    result = generate_property_code("int", "x")
    assert "setX" in result