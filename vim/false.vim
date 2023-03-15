" Vim syntax file
" Language:	false

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn keyword	fTodo		contained TODO FIXME XXX
syn cluster	fCommentGroup   contains=fTodo

syn region	fComment	start="{" end="}" contains=@fCommentGroup
syn match	fNumber		"[0-9]\+"
syn match	fVariable	"[a-z]"
syn match	fCharacter	"'."
syn region	fString		start="\"" end="\""
syn match  	fConditional	"?"
syn match  	fRepeat		"#"
syn match	fShebang	/^\%1l#!.*/

hi def link	fTodo		Todo
hi def link	fComment	Comment
hi def link	fNumber		Constant
hi def link	fVariable	PreProc
hi def link	fCharacter	PreProc
hi def link	fString		String
hi def link	fConditional	Statement
hi def link	fRepeat		Statement
hi def link	fShebang	Comment
