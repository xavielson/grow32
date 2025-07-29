#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

/**
 * Inicializa a conexão WiFi e o servidor web.
 * Chame no setup().
 */
void webinterface_setup();

/**
 * Mantém o servidor web rodando.
 * Chame regularmente no loop().
 */
void webinterface_loop();

#endif
