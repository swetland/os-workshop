extern crate proc_macro;
use proc_macro::TokenStream;

use quote::{format_ident, quote};
use syn::{parse::Parse, parse_macro_input, Expr, Ident, Token};

#[derive(Debug)]
struct CsrArgs {
    register: Ident,
    value: Expr,
}

impl Parse for CsrArgs {
    fn parse(input: syn::parse::ParseStream) -> syn::Result<Self> {
        let register: Ident = input.parse()?;
        input.parse::<Token![,]>()?;
        let value: Expr = input.parse()?;
        Ok(CsrArgs { register, value })
    }
}

#[proc_macro]
pub fn csr_read(tokens: TokenStream) -> TokenStream {
    let register = format_ident!("{}", tokens.to_string());
    let stream = quote!({
        let mut x: u32 = core::u32::MAX;
        unsafe {
            asm!("csrr {0}, {register}", out(reg) x, register = const #register);
        }
        x
    });

    TokenStream::from(stream)
}

#[proc_macro]
pub fn csr_write(tokens: TokenStream) -> TokenStream {
    let args = parse_macro_input!(tokens as CsrArgs);
    let register = args.register;
    let val = args.value;
    let stream = quote!({
        unsafe {
            asm!("csrw {register}, {val}", register = const #register, val = in(reg) #val);
        }
    });

    TokenStream::from(stream)
}

#[proc_macro]
pub fn csr_set(tokens: TokenStream) -> TokenStream {
    let args = parse_macro_input!(tokens as CsrArgs);
    let register = args.register;
    let val = args.value;

    let stream = quote!({
        unsafe {
            asm!("csrs {register}, {val}", register = const #register, val = in(reg) #val);
        }
    });

    TokenStream::from(stream)
}
