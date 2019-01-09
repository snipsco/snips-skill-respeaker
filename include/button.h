#ifndef __BUTTON_H__
#define __BUTTON_H__

#define LONG_PRESS_SEC 2
/**
 * @brief: Create a button from GPIO port
 *
 * @param[in] pin: GPIO port number
 * @param[in] short_callback: callback function for a short press detection
 * @param[in] long_callback: callback function for a long press detection
 *
 * @returns: -1/ Error
 *            0/ Success
 */
int Init_Key(int pin, void (*short_callback)(void), void (*long_callback)(void) );

/**
 * @brief: Release the GPIO port which is used for the button and kill the refreshing process
 *
 * @returns: -1/ Error
 *            0/ Success
 */
int Destroy_Key(void);

#endif