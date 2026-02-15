"""Tests for FNV-1a 32-bit hash function."""

from ipcgen.types import fnv1a_32


class TestFnv1a:
    """FNV-1a 32-bit hash function correctness."""

    def test_empty_string(self):
        """Empty string returns the FNV offset basis."""
        assert fnv1a_32("") == 0x811c9dc5

    def test_known_value(self):
        """DeviceMonitor hash matches the value the generator embeds."""
        assert fnv1a_32("DeviceMonitor") == 0x00fefaf3

    def test_different_names_differ(self):
        """Two distinct names produce different hashes."""
        assert fnv1a_32("Foo") != fnv1a_32("Bar")
