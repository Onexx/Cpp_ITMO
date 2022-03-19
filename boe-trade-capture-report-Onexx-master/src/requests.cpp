#include "requests.h"

namespace {

void encode_new_order_opt_fields(unsigned char * bitfield_start,
                                 const double price,
                                 const char ord_type,
                                 const char time_in_force,
                                 const unsigned max_floor,
                                 const std::string & symbol,
                                 const char capacity,
                                 const std::string & account)
{
    auto * p = bitfield_start + new_order_bitfield_num();
#define FIELD(name, bitfield_num, bit)                    \
    set_opt_field_bit(bitfield_start, bitfield_num, bit); \
    p = encode_field_##name(p, name);
#include "new_order_opt_fields.inl"
}

uint8_t encode_request_type(const RequestType type)
{
    switch (type) {
    case RequestType::New:
        return 0x38;
    case RequestType::TradeCapture:
        return 0x3C;
    }
    return 0;
}

unsigned char * add_request_header(unsigned char * start, unsigned length, const RequestType type, unsigned seq_no)
{
    *start++ = 0xBA;
    *start++ = 0xBA;
    start = encode(start, static_cast<uint16_t>(length));
    start = encode(start, encode_request_type(type));
    *start++ = 0;
    return encode(start, seq_no);
}

char convert_side(const Side side)
{
    switch (side) {
    case Side::Buy: return '1';
    case Side::Sell: return '2';
    }
    return 0;
}

char convert_ord_type(const OrdType ord_type)
{
    switch (ord_type) {
    case OrdType::Market: return '1';
    case OrdType::Limit: return '2';
    case OrdType::Pegged: return 'P';
    }
    return 0;
}

char convert_time_in_force(const TimeInForce time_in_force)
{
    switch (time_in_force) {
    case TimeInForce::Day: return '0';
    case TimeInForce::IOC: return '3';
    case TimeInForce::GTD: return '6';
    }
    return 0;
}

char convert_capacity(const Capacity capacity)
{
    switch (capacity) {
    case Capacity::Agency: return 'A';
    case Capacity::Principal: return 'P';
    case Capacity::RisklessPrincipal: return 'R';
    }
    return 0;
}

char convert_party_role(const PartyRole party_role)
{
    switch (party_role) {
    case PartyRole::ExecutingFirm: return '1';
    case PartyRole::EnteringFirm: return '2';
    case PartyRole::ContraFirm: return '3';
    }
    return 0;
}

char convert_def_pub(const bool deferred_publication)
{
    return deferred_publication ? 0x2 : 0x1;
}

unsigned char * encode_side(unsigned char * p, const Side side, const Capacity capacity, const std::string & party_id, const PartyRole party_role)
{
    p = encode_char(p, convert_side(side));
    p = encode_field_capacity(p, convert_capacity(capacity));
    p = encode_text(p, party_id, 4);
    p = encode_field_party_role(p, convert_party_role(party_role));
    return p;
}

} // anonymous namespace

std::array<unsigned char, calculate_size(RequestType::New)> create_new_order_request(const unsigned seq_no,
                                                                                     const std::string & cl_ord_id,
                                                                                     const Side side,
                                                                                     const double volume,
                                                                                     const double price,
                                                                                     const OrdType ord_type,
                                                                                     const TimeInForce time_in_force,
                                                                                     const double max_floor,
                                                                                     const std::string & symbol,
                                                                                     const Capacity capacity,
                                                                                     const std::string & account)
{
    static_assert(calculate_size(RequestType::New) == 78, "Wrong New Order message size");

    std::array<unsigned char, calculate_size(RequestType::New)> msg;
    auto * p = add_request_header(&msg[0], msg.size() - 2, RequestType::New, seq_no);
    p = encode_text(p, cl_ord_id, 20);
    p = encode_char(p, convert_side(side));
    p = encode_binary4(p, static_cast<uint32_t>(volume));
    p = encode(p, static_cast<uint8_t>(new_order_bitfield_num()));
    encode_new_order_opt_fields(p,
                                price,
                                convert_ord_type(ord_type),
                                convert_time_in_force(time_in_force),
                                max_floor,
                                symbol,
                                convert_capacity(capacity),
                                account);
    return msg;
}

std::vector<unsigned char> create_trade_capture_report_request(
        unsigned seq_no,
        const std::string & trade_report_id,
        double volume,
        double price,
        const std::string & party_id,
        Side side,
        Capacity capacity,
        const std::string & contra_party_id,
        Capacity contra_capacity,
        const std::string & symbol,
        bool deferred_publication)
{
    static_assert(calculate_size(RequestType::TradeCapture) == 70, "Wrong New Order message size");

    std::vector<unsigned char> msg(calculate_size(RequestType::TradeCapture));
    auto * p = add_request_header(&msg[0], msg.size() - 2, RequestType::TradeCapture, seq_no);
    p = encode_text(p, trade_report_id, 20);
    p = encode_binary4(p, static_cast<uint32_t>(volume));
    p = encode_price(p, price * 1000);

    p = encode(p, static_cast<uint8_t>(trade_capture_bitfield_num()));

#define FIELD(_, bitfield_num, bit) \
    set_opt_field_bit(p, bitfield_num, bit);
#include "trade_capture_opt_fields.inl"
    p += new_order_bitfield_num();

    p = encode_char(p, 2);

    p = encode_side(p, side, capacity, party_id, PartyRole::EnteringFirm);

    Side contra_side = (side == Side::Buy ? Side::Sell : Side::Buy);

    p = encode_side(p, contra_side, contra_capacity, contra_party_id, PartyRole::ContraFirm);

    p = encode_field_symbol(p, symbol);
    p = encode_field_trade_publish_indicator(p, convert_def_pub(deferred_publication));
    return msg;
}
