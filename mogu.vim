" Vim syntax file
" Language: MoguScript
" Maintainer: Tom A. Thorogood
" Latest Revision: 15 September 2012

if exists("b:current_syntax")
    finish
endif

syn keyword moguReserved own self 

" Mouse-Related
syn keyword moguReserved click clicked mouseover mouseout 

" Boolean related
syn keyword moguReserved failed succeeded test error reported

" Keyboard related
syn keyword moguReserved enter pressed keyup

" States of Change
syn keyword moguReserved hidden index style css changed loaded 

" Actions
syn keyword moguReserved increment decrement email reload emit append
syn keyword moguReserved set reset remove clear script

" Attributes
syn keyword moguReserved tooltip path regex slot source location
syn keyword moguReserved content text contents member sort
syn keyword moguReserved default key encrypted mode type ascending descending
syn keyword moguReserved properties

" Data Types
syn keyword moguReserved list string hash

" Widget Types
syn keyword moguReserved link image submit radio textarea input dropdown
syn keyword moguReserved multi container password stack  

" Prepositions
syn keyword humanSyntax to as at with from for is using

"Object Types 
syn keyword moguObject data user group validator widget template policy
syn keyword moguObject perspective meta

syn keyword moguOperators ( ) + - * / { } : <<< >>>

" Operators
syn keyword scriptOperator when end

syn keyword subBlock events children

" Variable constructs
syn match identifier "[a-zA-Z_:][a-zA-Z0-9:_]\+"
syn match number "\d+(\.\d+)?"
syn match preproc_ref "@[a-zA-Z_:][a-zA-Z0-9:_]\+"
syn match special_kw "center-vertically"
syn match comment "^#.*"
syn region stringliteral start=+"+  end=+"+  skip=+\\"+
syn region commandblock start="{" end="}" fold transparent contains=moguReserved,identifier,number,stringliteral,moguObject,comment,preproc_ref
syn match md_internal ".+" contained
syn region markdown start="<<<" end=">>>" contains=NONE

hi def link moguReserved    Type
hi def link special_kw      Type
hi def link identifier      Identifier
hi def link number          Constant
hi def link stringliteral   Constant
hi def link comment         Comment
hi def link preproc_ref     Comment
hi def link md_internal     Comment
hi def link markdown        Comment
hi def link moguObject      Type       
hi def link scriptOperator  Operator
hi def link moguOperators   Operator
hi def link subBlock        Ignore
hi def link humanSyntax     Ignore 
