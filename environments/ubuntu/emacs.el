;;; .emacs --- config file

;;; Commentary:

;;; Code:

(add-to-list 'default-frame-alist '(background-color . "snow2"))

(fringe-mode (cons 1 0))

;; Remove all trailing whitespace on save
(add-hook 'before-save-hook 'delete-trailing-whitespace)

;; Changes all yes/no questions to y/n type
(fset 'yes-or-no-p 'y-or-n-p)

;; Use shift-arrows to navigate windows
(windmove-default-keybindings)

(setq echo-keystrokes 0.1)
(setq vc-follow-symlinks t)

(add-to-list 'load-path "~/.emacs.d/lisp/")

;; banish ui shits
(if (fboundp 'tool-bar-mode) (tool-bar-mode -1))
(if (fboundp 'menu-bar-mode) (menu-bar-mode -1))

(setq custom-file "~/.emacs.d/custom.el")
(load custom-file)

(setq-default auto-fill-function 'do-auto-fill)
(setq-default fill-column 100)
(turn-on-auto-fill)
;; Disable auto-fill-mode in programming mode
(add-hook 'prog-mode-hook (lambda () (auto-fill-mode -1)))

(fido-mode 1)
(define-key icomplete-fido-mode-map (kbd "SPC") 'self-insert-command)

;; Package shit
;;
;; Bootstrap straight.el
(defvar bootstrap-version)
(let ((bootstrap-file
       (expand-file-name "straight/repos/straight.el/bootstrap.el" user-emacs-directory))
      (bootstrap-version 5))
  (unless (file-exists-p bootstrap-file)
    (with-current-buffer
        (url-retrieve-synchronously
         "https://raw.githubusercontent.com/raxod502/straight.el/develop/install.el"
         'silent 'inhibit-cookies)
      (goto-char (point-max))
      (eval-print-last-sexp)))
  (load bootstrap-file nil 'nomessage))

;;;;  Effectively replace use-package with straight-use-package
;;; https://github.com/raxod502/straight.el/blob/develop/README.md#integration-with-use-package
(setq straight-use-package-by-default t)
(straight-use-package 'use-package)

;; OK, really minimal use-package; see https://github.crookster.org/switching-to-straight.el-from-emacs-26-builtin-package.el/
;;;;  package.el
;;; so package-list-packages includes them
(require 'package)
(add-to-list 'package-archives
             '("melpa" . "https://melpa.org/packages/"))

(use-package markdown-mode
  :commands markdown-mode gfm-mode
  :mode (("README\\.md\\'" . gfm-mode)
         ("\\.md\\'" . gfm-mode)
         ("\\.markdown\\'" . markdown-mode))
  :init (setq markdown-command 'pandoc))


(use-package zoom-frm)

(use-package indent-tools)

(use-package which-key
  :init (which-key-mode))

(use-package smex
  :bind (
         ([remap execute-extended-command] . smex)
         ("M-X" . smex-major-mode-commands)
         )
  )

(use-package ibuffer                    ; Better buffer list
  :bind (([remap list-buffers] . ibuffer))
  ;; Show VC Status in ibuffer
  :config (setq ibuffer-formats
                '((mark modified read-only vc-status-mini " "
                        (name 18 18 :left :elide)
                        " "
                        (size 9 -1 :right)
                        " "
                        (mode 16 16 :left :elide)
                        " "
                        (vc-status 16 16 :left)
                        " "
                        filename-and-process)
                  (mark modified read-only " "
                        (name 18 18 :left :elide)
                        " "
                        (size 9 -1 :right)
                        " "
                        (mode 16 16 :left :elide)
                        " " filename-and-process)
                  (mark " "
                        (name 16 -1)
                        " " filename))))

(use-package hydra
  :init
  (defhydra hydra-zoom (global-map "<f2>")
    "zoom"
    ("g" text-scale-increase)
    ("l" text-scale-decrease)))

(use-package expand-region
  :bind ("C-=" . er/expand-region))

(use-package popper
  :bind
  (("C-'" . popper-toggle-latest)
   ("M-'" . popper-cycle)
   ("C-\"" . popper-toggle-type))
  :init
  (setq popper-reference-buffers
        '("\\*Messages\\*"
          "Output\\*$"
          help-mode
          compilation-mode))
  (popper-mode +1))

(use-package magit
  )

(use-package git-timemachine)

(use-package undo-tree
  :init (global-undo-tree-mode)
  :bind (("C-z" . undo)
         ("C-S-z" . redo)))

(use-package company
  :bind ((:map company-active-map
	           ("C-n". company-select-next)
	           ("C-p". company-select-previous)
	           ("M-<". company-select-first)
	           ("M->". company-select-last))
         (:map company-mode-map
	           ("<tab>". tab-indent-or-complete)
	           ("TAB". tab-indent-or-complete)))
  :config
  (progn
    (setq company-tooltip-align-annotations t
          ;; Easy navigation to candidates with M-<n>
          company-show-numbers t)
    (setq company-idle-delay 0.1)
    (setq company-minimum-prefix-length 1)
    (company-tng-configure-default)
    (setq company-frontends '(company-tng-frontend
                              company-pseudo-tooltip-frontend
                              company-echo-metadata-frontend)))
  :diminish company-mode)

(global-company-mode)
(dolist (key '("<return>" "RET"))
  ;; Here we are using an advanced feature of define-key that lets
  ;; us pass an "extended menu item" instead of an interactive
  ;; function. Doing this allows RET to regain its usual
  ;; functionality when the user has not explicitly interacted with
  ;; Company.
  (define-key company-active-map (kbd key)
    `(menu-item nil company-complete
                :filter ,(lambda (cmd)
                           (when (company-explicit-action-p)
                             cmd)))))
(define-key company-active-map (kbd "TAB") #'company-complete-selection)
;;(define-key company-active-map (kbd "SPC") nil)
;; Company appears to override the above keymap based on company-auto-complete-chars.
;; Turning it off ensures we have full control.
(setq company-auto-complete-chars nil)

(use-package yasnippet
  :config
  (yas-reload-all)
  (add-hook 'prog-mode-hook 'yas-minor-mode)
  (add-hook 'text-mode-hook 'yas-minor-mode))

(defun company-yasnippet-or-completion ()
  (interactive)
  (or (do-yas-expand)
      (company-complete-common)))

(defun check-expansion ()
  (save-excursion
    (if (looking-at "\\_>") t
      (backward-char 1)
      (if (looking-at "\\.") t
        (backward-char 1)
        (if (looking-at "::") t nil)))))

(defun do-yas-expand ()
  (let ((yas/fallback-behavior 'return-nil))
    (yas/expand)))

(defun tab-indent-or-complete ()
  (interactive)
  (if (minibufferp)
      (minibuffer-complete)
    (if (or (not yas/minor-mode)
            (null (do-yas-expand)))
        (if (check-expansion)
            (company-complete-common)
          (indent-for-tab-command)))))

(use-package flycheck
  :init (global-flycheck-mode))

(use-package tree-sitter
  :config
  (global-tree-sitter-mode 1))
(use-package tree-sitter-langs)

;; LSP stuff
(use-package lsp-mode
  :commands lsp
  ;; :bind (("M-." . xref-find-definitions))
  :custom
  ;; what to use when checking on-save. "check" is default, I prefer clippy
  (lsp-rust-analyzer-cargo-watch-command "clippy")
  (lsp-eldoc-render-all t)
  (lsp-idle-delay 0.2)
  (lsp-enable-snippet t)
  :hook
  ((c-mode . lsp)
   (lsp-mode . lsp-enable-which-key-integration)))

;; C-lang shit
(use-package lsp-clangd
  :bind (:map lsp-mode-map
              ("M-j" . lsp-ui-imenu)
              ("M-?" . lsp-find-references)
              ("C-c C-c l" . flycheck-list-errors)
              ("C-c C-c a" . lsp-execute-code-action)
              ("C-c C-c r" . lsp-rename)
              ("C-c C-c q" . lsp-workspace-restart)
              ("C-c C-c Q" . lsp-workspace-shutdown))
  :config
  ;; uncomment for less flashiness
  ;; (setq lsp-eldoc-hook nil)
  ;; (setq lsp-enable-symbol-highlighting nil)
  ;; (setq lsp-signature-auto-activate nil)

  ;; comment to disable rustfmt on save
  (add-hook 'c-mode-hook 'nebkor/clangd-mode-hook))

(defun nebkor/clangd-mode-hook ()
  (setq-local buffer-save-without-query t)
  (tree-sitter-hl-mode 1)
  (setq lsp-semantic-tokens-enable nil))

(use-package lsp-ui
  :commands lsp-ui-mode
  )

(use-package lsp-treemacs
  :commands lsp-treemacs-errors-list
  :config
  (setq lsp-treemacs-errors-position-params `((side . left)
                                              (slot . 3)
                                              (window-width . ,treemacs-width)))
  (add-hook 'treemacs-mode-hook (lambda() (company-mode -1)))
  )

(use-package dap-mode
  :config
  (dap-ui-mode)
  (dap-ui-controls-mode 1)

  (require 'dap-gdb-lldb)
  ;; installs .extension/vscode
  (dap-gdb-lldb-setup)
  (dap-register-debug-template
   "Rust::LLDB Run Configuration"
   (list :type "gdb"
         :request "launch"
         :name "GDB::Run"
	     :gdbpath "rust-gdb"
         :target nil
         :cwd nil)))

(put 'magit-diff-edit-hunk-commit 'disabled nil)

(provide '.emacs)

;;; -*-Emacs-Lisp-*-

;;; .emacs ends here
