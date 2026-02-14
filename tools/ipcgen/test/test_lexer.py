"""Tests for the IDL lexer (tokenizer)."""

import pytest

from ipcgen.lexer import (
    tokenize,
    TOK_KEYWORD,
    TOK_IDENT,
    TOK_NUMBER,
    TOK_SYMBOL,
    TOK_ATTR,
    TOK_EOF,
)


class TestLexer:
    """Tokenization of IDL text into token streams."""

    def test_empty_input(self):
        """Empty string produces only an EOF token."""
        tokens = tokenize("")
        assert len(tokens) == 1
        assert tokens[0].kind == TOK_EOF

    def test_keywords(self):
        """Reserved words are recognized as KEYWORD tokens."""
        tokens = tokenize("service notifications int void")
        kinds = [t.kind for t in tokens if t.kind != TOK_EOF]
        assert kinds == [TOK_KEYWORD] * 4
        values = [t.value for t in tokens if t.kind != TOK_EOF]
        assert values == ["service", "notifications", "int", "void"]

    def test_identifiers(self):
        """Non-keyword words are recognized as IDENT tokens."""
        tokens = tokenize("Foo myVar_1 _under")
        idents = [t for t in tokens if t.kind == TOK_IDENT]
        assert [t.value for t in idents] == ["Foo", "myVar_1", "_under"]

    def test_numbers(self):
        """Digit sequences are recognized as NUMBER tokens."""
        tokens = tokenize("123 42 0")
        nums = [t for t in tokens if t.kind == TOK_NUMBER]
        assert [t.value for t in nums] == ["123", "42", "0"]

    def test_symbols(self):
        """Punctuation characters are recognized as SYMBOL tokens."""
        tokens = tokenize("{ } ( ) ; , *")
        syms = [t for t in tokens if t.kind == TOK_SYMBOL]
        assert [t.value for t in syms] == ["{", "}", "(", ")", ";", ",", "*"]

    def test_attributes(self):
        """Bracketed attributes are recognized as ATTR tokens."""
        tokens = tokenize("[method=1] [in] [out] [notify=2]")
        attrs = [t for t in tokens if t.kind == TOK_ATTR]
        assert [t.value for t in attrs] == ["method=1", "in", "out", "notify=2"]

    def test_whitespace_and_newlines(self):
        """Whitespace is skipped; line numbers are tracked correctly."""
        tokens = tokenize("Foo\n\nBar\r\n\tBaz")
        idents = [t for t in tokens if t.kind == TOK_IDENT]
        assert [t.value for t in idents] == ["Foo", "Bar", "Baz"]
        assert idents[0].line == 1
        assert idents[1].line == 3
        assert idents[2].line == 4

    def test_single_line_comment(self):
        """Single-line comments are skipped entirely."""
        tokens = tokenize("Foo // this is a comment\nBar")
        idents = [t for t in tokens if t.kind == TOK_IDENT]
        assert [t.value for t in idents] == ["Foo", "Bar"]

    def test_block_comment(self):
        """Block comments are skipped; line count is updated."""
        tokens = tokenize("Foo /* multi\nline\ncomment */ Bar")
        idents = [t for t in tokens if t.kind == TOK_IDENT]
        assert [t.value for t in idents] == ["Foo", "Bar"]
        # "Bar" should be on line 3 (comment spans 2 newlines).
        assert idents[1].line == 3

    def test_unterminated_block_comment(self):
        """Unterminated block comment raises SyntaxError."""
        with pytest.raises(SyntaxError, match="unterminated block comment"):
            tokenize("Foo /* never closed")

    def test_unterminated_attribute(self):
        """Unterminated attribute bracket raises SyntaxError."""
        with pytest.raises(SyntaxError, match="unterminated attribute"):
            tokenize("[method=1")

    def test_unexpected_character(self):
        """Unexpected characters raise SyntaxError."""
        with pytest.raises(SyntaxError, match="unexpected character"):
            tokenize("@")
