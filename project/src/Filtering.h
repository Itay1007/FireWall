char * str_of_table(void);
char * str_of_table_ent(int i);
int verdict_packet(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
int verdict_paket_with_rule(void *priv, struct sk_buff *skb, const struct nf_hook_state *state, rule_t rule);