#include <stdio.h>

#include "nmea_parser.h"

//! Just as an example, we count the number of PARTICLE messages per frame
int particle_messages_count = 0;
//! Keeps track of the current frame number
int framenumber = 0;

void nmea_process_message(struct sNMEAMessage *m, int withchecksum) {
	/* This method is called each time a complete message was received (with either a valid or no checksum).
	 * A message consists of a command string and arguments. The command string can be accessed via
	 *     m->command
	 * while the arguments can be access with
	 *     unsigned char *arg = m->argument[i];
	 * The number of arguments is
	 *     int numarg = m->argument_count;
	 */

	if (strcmp(m->command, "STEP_START") == 0) {
		// This message indicates the beginning of a new frame
		framenumber = 0;
		particle_messages_count = 0;
	} else if (strcmp(m->command, "FRAMENUMBER") == 0) {
		// This message contains the frame number
		framenumber = 0;
		if (m->argument[0]) {
			framenumber = atoi(m->argument[0]);
		}
	} else if (strcmp(m->command, "PARTICLE") == 0) {
		// This message transmits the position of a particle for the current frame
		particle_messages_count++;
	} else if (strcmp(m->command, "STEP_STOP") == 0) {
		// This message indicates the end of the current frame
		printf("Got %d particle(s) for frame %d\r\n", particle_messages_count, framenumber);
	} else {
		printf("Unknown message '%s' received!\r\n", m->command);
	}
}

void nmea_process_message_checksum_error(struct sNMEAMessage *m) {
	/* We don't really expect messages with checksum errors here.
	 * Hence, we just discard such messages.
	 *
	 * In case you are working with lossy channels, you could do something more fancy here.
	 */
}

void nmea_process_unrecognized_char(char c) {
	/* Unrecognized chars are bytes that are transmitted between valid NMEA messages.
	 * An NMEA stream usually only consists of NMEA messages, but people sometimes put human readable comments in between.
	 * On lossy channels, messages missing the $ sign (start-of-message) would also fall in here.
	 *
	 * This method is called for each char that does not belong to an NMEA message.
	 * But unless you have a good reason, leave this method empty.
	 */
}

void nmea_send(const char *buffer, int len) {
	/* Whenever you send a message using NMEASendMessage(CommunicationMessage *m),
	 * this method is called to actually transmit the bytes.
	 */

	// This is just useless cosmetics, as we never send stuff back to SwisTrack in this simple example
	write(0, buffer, len);
}

//! Main program
int main(int argc, char *argv) {
	char buffer[128];
	struct sNMEAParser parser;

	printf("Go ahead and give me some NMEA messages, e.g.:\r\n");
	printf("  $STEP_START\r\n");
	printf("  $FRAMENUMBER,34\r\n");
	printf("  $PARTICLE,0,0,0,0\r\n");
	printf("  $STEP_STOP\r\n");
	printf("\r\n");

	// Initialize a parser
	nmea_parser_init(&parser);
	parser.hook_process_message = &nmea_process_message;
	parser.hook_process_message_checksum_error = &nmea_process_message_checksum_error;
	parser.hook_process_unrecognized_char = &nmea_process_unrecognized_char;
	parser.hook_send = &nmea_send;

	// As long as we can
	while (1) {
		// Read available bytes
		int len = read(0, buffer, 128);
		if (len < 0) {
			printf("STDIN closed ...\r\n");
			break;
		}

		// Pass it to the NMEA parser (this will subsequently call our OnNMEA... methods)
		nmea_parser_process_data(&parser, buffer, len);
	}

	return 0;
}
