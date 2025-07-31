#ifndef WEB_ROUTES_H
#define WEB_ROUTES_H

/**
 * Configura todas as rotas REST/HTTP do WebServer.
 * Deve ser chamada após conectar o WiFi.
 */
void setupWebRoutes();

/**
 * Mantém o servidor HTTP respondendo.
 * Deve ser chamada dentro do loop().
 */
void handleWebServer();

#endif
