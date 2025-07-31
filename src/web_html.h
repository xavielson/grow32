#ifndef WEB_HTML_H
#define WEB_HTML_H

#include <Arduino.h>

/**
 * Escapa caracteres especiais para HTML.
 * Ex: < vira &lt;, > vira &gt;, etc.
 * Útil para garantir segurança e correto render do nome/type enviados do backend.
 */
String htmlEscape(const String& str);

/**
 * Retorna toda a página HTML do frontend do sistema.
 * Inclui todo o CSS, JS, e modais necessários para uso.
 * O parâmetro debug ativa o modo debug visual no frontend.
 */
String getPage(bool debug);

#endif
