#include <stdio.h>
#include <string.h>
#include "nmea_parser.h"

void nmea_parser_init(struct sNMEAParser *parser) {
	parser->receive_buffer_cur = 0;
	parser->state = cNMEAParserState_None;

	parser->hook_process_message = 0;
	parser->hook_process_message_checksum_error = 0;
	parser->hook_process_unrecognized_char = 0;
	parser->hook_send = 0;

	nmea_message_init(&(parser->message));
}

void nmea_message_init(struct sNMEAMessage *message) {
	int i;
	message->command = 0;
	message->argument_count = 0;
	for (i = 0; i < NMEA_MESSAGE_ARGUMENTS_MAX; i++) {
		message->argument[i] = 0;
	}
}

void nmea_parser_process_data(struct sNMEAParser *parser, const char *buffer, int len) {
	int i;
	char inchar;
	char checksum_expected;

	for (i = 0; i < len; i++) {
		inchar = buffer[i];
		if (parser->state == cNMEAParserState_None) {
			if (inchar == '$') {
				parser->receive_buffer_cur = 0;
				nmea_message_init(&(parser->message));
				parser->message.command = &(parser->receive_buffer[parser->receive_buffer_cur]);
				parser->checksum = 0;
				parser->state = cNMEAParserState_Argument;
			} else {
				if (parser->hook_process_unrecognized_char) {
					parser->hook_process_unrecognized_char(inchar);
				}
			}
		} else if (parser->state == cNMEAParserState_Argument) {
			if (inchar == ',') {
				parser->checksum = parser->checksum ^ inchar;
				parser->receive_buffer[parser->receive_buffer_cur] = 0;
				parser->receive_buffer_cur++;
				parser->message.argument[parser->message.argument_count] = &(parser->receive_buffer[parser->receive_buffer_cur]);
				parser->message.argument_count++;
			} else if (inchar == '*') {
				parser->receive_buffer[parser->receive_buffer_cur] = 0;
				parser->receive_buffer_cur++;
				parser->state = cNMEAParserState_Checksum1;
			} else if ((inchar == '\n') || (inchar == '\r')) {
				parser->receive_buffer[parser->receive_buffer_cur] = 0;
				parser->receive_buffer_cur++;
				if (parser->hook_process_message) {
					parser->hook_process_message(&(parser->message), 0);
				}
				parser->state = cNMEAParserState_None;
			} else {
				parser->receive_buffer[parser->receive_buffer_cur++] = inchar;
				parser->checksum = parser->checksum ^ inchar;
			}
		} else if (parser->state == cNMEAParserState_Checksum1) {
			checksum_expected = "0123456789ABCDEF"[(parser->checksum >> 4) & 0xF];
			if (inchar == '\n') {
				if (parser->hook_process_message_checksum_error) {
					parser->hook_process_message_checksum_error(&(parser->message));
				}
				parser->state = cNMEAParserState_None;
			} else if (inchar == checksum_expected) {
				parser->state = cNMEAParserState_Checksum2;
			} else {
				if (parser->hook_process_message_checksum_error) {
					parser->hook_process_message_checksum_error(&(parser->message));
				}
				parser->state = cNMEAParserState_Checksum2Discard;
			}
		} else if (parser->state == cNMEAParserState_Checksum2) {
			checksum_expected = "0123456789ABCDEF"[parser->checksum & 0xF];
			if (inchar == checksum_expected) {
				if (parser->hook_process_message) {
					parser->hook_process_message(&(parser->message), 1);
				}
			} else {
				if (parser->hook_process_message_checksum_error) {
					parser->hook_process_message_checksum_error(&(parser->message));
				}
			}
			parser->state = cNMEAParserState_None;
		} else if (parser->state == cNMEAParserState_Checksum2Discard) {
			parser->state = cNMEAParserState_None;
		}
	}
}

void nmea_parser_send(struct sNMEAParser *parser, struct sNMEAMessage *message) {
	int i;
	int len = 0;

	// If we cannot send anything, then quit
	if (! parser->hook_send) {
		return;
	}

	// Start sequence
	parser->hook_send("$", 1);

	// Command
	if (message->command) {
		parser->hook_send(message->command, strlen(message->command));
	}

	// Arguments
	for (i = 0; i < NMEA_MESSAGE_ARGUMENTS_MAX; i++) {
		if (message->argument[i]) {
			parser->hook_send(",", 1);
			parser->hook_send(message->argument[i], strlen(message->argument[i]));
		}
	}

	// End
	parser->hook_send("\r\n", 2);
}
