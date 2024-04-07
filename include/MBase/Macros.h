#pragma once

#ifndef DELETEARR
#define DELETEARR(arr) do {if (arr){ delete [] (arr); (arr) = NULL; }} while (0)
#endif

#ifndef DELETEPTR
#define DELETEPTR(p) do {if (p){ delete (p); (p) = nullptr; }} while (0)
#endif