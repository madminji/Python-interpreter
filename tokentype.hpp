#ifndef TOKENTYPE_HPP
#define TOKENTYPE_HPP

enum TokenType {
    Keyword,
    Class, Print, Def, Return,
    While, Break, Continue,
    If, Elif, Else,
    True, False, None,
    And, Or, Not,
    Pass,

    Operator,
    Plus, Minus,
    Star, DoubleStar,
    Slash, DoubleSlash, Mod,
    Pipe, Caret, Ampersand, Tilde,
    LeftShift, RightShift,
    Less, LessEqual,
    Greater, GreaterEqual,
    Equals, EqualEqual,
    Bang, BangEqual,
    PlusEqual, MinusEqual,
    StarEqual, SlashEqual, ModEqual,
    AndEqual, OrEqual, XorEqual,
    LeftShiftEqual,
    RightShiftEqual,
    Dot,

    Punctuator,
    LeftParen, RightParen,
    LeftBrace, RightBrace,
    Comma, Colon, Semicolon,
    Indent, Dedent,
    Newline,
    EndOfFile,

    Identifier,
    Literal,
    Int, Float, Name, String,

    // Others
    Error,
};
#endif // TOKENTYPE_HPP#pragma once
