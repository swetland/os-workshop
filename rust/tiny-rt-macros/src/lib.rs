extern crate proc_macro;
use proc_macro::TokenStream;

use quote::quote;
use syn::{parse::Parse, parse_macro_input, Expr, Ident, Token};

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
    //
    // let x = unsafe { csr_read!(REGISTER_NAME) };
    //
    let register = parse_macro_input!(tokens as Ident);

    let stream = quote!({
        let x;
        asm!("csrr {0}, {register}", out(reg) x, register = const #register);
        x
    });

    TokenStream::from(stream)
}

#[proc_macro]
pub fn csr_write(tokens: TokenStream) -> TokenStream {
    //
    // unsafe {`csr_write!(REGISTER_NAME, value_expression); }
    //
    let args = parse_macro_input!(tokens as CsrArgs);
    let register = args.register;
    let val = args.value;

    let stream = quote!({
        asm!("csrw {register}, {val}", register = const #register, val = in(reg) #val);
    });

    TokenStream::from(stream)
}

#[proc_macro]
pub fn csr_set(tokens: TokenStream) -> TokenStream {
    //
    // unsafe { csr_set!(REGISTER_NAME, value_expression); }
    //
    let args = parse_macro_input!(tokens as CsrArgs);
    let register = args.register;
    let val = args.value;

    let stream = quote!({
        asm!("csrs {register}, {val}", register = const #register, val = in(reg) #val);
    });

    TokenStream::from(stream)
}
