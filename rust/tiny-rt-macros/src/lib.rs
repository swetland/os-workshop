extern crate proc_macro;
use proc_macro::TokenStream;

use proc_macro2::TokenStream as TS2;

use quote::format_ident;
use quote::quote;
use syn::{parse_macro_input, Ident};

#[proc_macro]
pub fn csr_read_macro(tokens: TokenStream) -> TokenStream {
    let template = tokens.to_string();
    let stream = quote!({
        fn __rdfn() -> u32 {
            let mut x: u32 = 0;
            unsafe {
                asm!(#template, out(reg) x);
            }
            x
        }

        __rdfn()
    });
    TokenStream::from(stream)
}
