#ifndef NMEA_PARSER
#define NMEA_PARSER

#define NMEA_MESSAGE_ARGUMENTS_MAX 16
#define NMEA_PARSER_RECEIVE_BUFFER_SIZE 128

//! NMEA Message
struct sNMEAMessage {
	char *command;								//!< Message command
	char *argument[NMEA_MESSAGE_ARGUMENTS_MAX];	//!< Message arguments
	unsigned int argument_count;				//!< Number of arguments
};

//! NMEA Parser States
enum eNMEAParserState {
	cNMEAParserState_None,
	cNMEAParserState_Argument,
	cNMEAParserState_Checksum1,
	cNMEAParserState_Checksum2,
	cNMEAParserState_Checksum2Discard
};

//! NMEA Parser
struct sNMEAParser {
	char receive_buffer[NMEA_PARSER_RECEIVE_BUFFER_SIZE];					//!< (parser state) Receive buffer
	int receive_buffer_cur;													//!< (parser state) Receive buffer position
	int checksum;															//!< (parser state) Checksum
	enum eNMEAParserState state;											//!< (parser state) State

	struct sNMEAMessage message;											//!< Current message
	void (*hook_process_message)(struct sNMEAMessage *m, int withchecksum);	//!< Called when a complete message has been received
	void (*hook_process_message_checksum_error)(struct sNMEAMessage *m);	//!< Called when a complete message has been received
	void (*hook_process_unrecognized_char)(char c);				//!< Called when a char outside a message is processed
	void (*hook_send)(const char *buffer, int len);				//!< Called when a char outside a message is processed
};


//! Initialization.
void nmea_parser_init(struct sNMEAParser *parser);
//! Parse incoming data.
void nmea_parser_process_data(struct sNMEAParser *parser, const char *buffer, int len);
//! Sends a message.
void nmea_parser_send(struct sNMEAParser *parser, struct sNMEAMessage *message);

//! Initializes a message.
void nmea_message_init(struct sNMEAMessage *message);

#endif
