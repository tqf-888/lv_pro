#ifndef KTV_LOG_IN_H
#define KTV_LOG_IN_H

void load_usr_token_from_file(void);
const char *get_usr_token(void);
void set_verification_code(const char *code);
void set_phone_num(const char *phone_num);

#endif // KTV_LOG_IN_H