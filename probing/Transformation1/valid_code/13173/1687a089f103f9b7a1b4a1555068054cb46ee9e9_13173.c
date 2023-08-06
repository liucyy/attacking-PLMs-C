VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_WARNING)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_WARNING_RET_CORUPT)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_WARNING_BUF_END_BEFORE_LE)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_WARNING_INVALID_FILE_SELECTED)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_WARNING_FCI_FORMAT_INVALID)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_WARNING_CHANGE)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_WARNING_FILE_FILLED)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_EXC_ERROR)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_EXC_ERROR_CHANGE)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_EXC_ERROR_MEMORY_FAILURE)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_WRONG_LENGTH)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_CLA_NOT_SUPPORTED)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_CHANNEL_NOT_SUPPORTED)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_SECURE_NOT_SUPPORTED)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_COMMAND_NOT_SUPPORTED)

VCARD_RESPONSE_NEW_STATIC_STATUS(

                    VCARD7816_STATUS_ERROR_COMMAND_INCOMPATIBLE_WITH_FILE)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_SECURITY_NOT_SATISFIED)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_AUTHENTICATION_BLOCKED)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_DATA_INVALID)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_CONDITION_NOT_SATISFIED)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_DATA_NO_EF)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_SM_OBJECT_MISSING)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_SM_OBJECT_INCORRECT)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_WRONG_PARAMETERS)

VCARD_RESPONSE_NEW_STATIC_STATUS(

                            VCARD7816_STATUS_ERROR_WRONG_PARAMETERS_IN_DATA)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_FUNCTION_NOT_SUPPORTED)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_FILE_NOT_FOUND)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_RECORD_NOT_FOUND)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_NO_SPACE_FOR_FILE)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_LC_TLV_INCONSISTENT)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_P1_P2_INCORRECT)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_LC_P1_P2_INCONSISTENT)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_DATA_NOT_FOUND)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_WRONG_PARAMETERS_2)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_INS_CODE_INVALID)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_CLA_INVALID)

VCARD_RESPONSE_NEW_STATIC_STATUS(VCARD7816_STATUS_ERROR_GENERAL)





VCardResponse *

vcard_make_response(vcard_7816_status_t status)

{

    VCardResponse *response = NULL;



    switch (status) {

    

    case VCARD7816_STATUS_SUCCESS:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_SUCCESS);

    case VCARD7816_STATUS_WARNING:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_WARNING);

    case VCARD7816_STATUS_WARNING_RET_CORUPT:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_WARNING_RET_CORUPT);

    case VCARD7816_STATUS_WARNING_BUF_END_BEFORE_LE:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_WARNING_BUF_END_BEFORE_LE);

    case VCARD7816_STATUS_WARNING_INVALID_FILE_SELECTED:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_WARNING_INVALID_FILE_SELECTED);

    case VCARD7816_STATUS_WARNING_FCI_FORMAT_INVALID:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_WARNING_FCI_FORMAT_INVALID);

    case VCARD7816_STATUS_WARNING_CHANGE:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_WARNING_CHANGE);

    case VCARD7816_STATUS_WARNING_FILE_FILLED:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_WARNING_FILE_FILLED);

    case VCARD7816_STATUS_EXC_ERROR:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_EXC_ERROR);

    case VCARD7816_STATUS_EXC_ERROR_CHANGE:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_EXC_ERROR_CHANGE);

    case VCARD7816_STATUS_EXC_ERROR_MEMORY_FAILURE:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_EXC_ERROR_MEMORY_FAILURE);

    case VCARD7816_STATUS_ERROR_WRONG_LENGTH:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_WRONG_LENGTH);

    case VCARD7816_STATUS_ERROR_CLA_NOT_SUPPORTED:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_CLA_NOT_SUPPORTED);

    case VCARD7816_STATUS_ERROR_CHANNEL_NOT_SUPPORTED:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_CHANNEL_NOT_SUPPORTED);

    case VCARD7816_STATUS_ERROR_SECURE_NOT_SUPPORTED:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_SECURE_NOT_SUPPORTED);

    case VCARD7816_STATUS_ERROR_COMMAND_NOT_SUPPORTED:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_COMMAND_NOT_SUPPORTED);

    case VCARD7816_STATUS_ERROR_COMMAND_INCOMPATIBLE_WITH_FILE:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_COMMAND_INCOMPATIBLE_WITH_FILE);

    case VCARD7816_STATUS_ERROR_SECURITY_NOT_SATISFIED:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_SECURITY_NOT_SATISFIED);

    case VCARD7816_STATUS_ERROR_AUTHENTICATION_BLOCKED:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_AUTHENTICATION_BLOCKED);

    case VCARD7816_STATUS_ERROR_DATA_INVALID:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_DATA_INVALID);

    case VCARD7816_STATUS_ERROR_CONDITION_NOT_SATISFIED:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_CONDITION_NOT_SATISFIED);

    case VCARD7816_STATUS_ERROR_DATA_NO_EF:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_DATA_NO_EF);

    case VCARD7816_STATUS_ERROR_SM_OBJECT_MISSING:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_SM_OBJECT_MISSING);

    case VCARD7816_STATUS_ERROR_SM_OBJECT_INCORRECT:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_SM_OBJECT_INCORRECT);

    case VCARD7816_STATUS_ERROR_WRONG_PARAMETERS:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_WRONG_PARAMETERS);

    case VCARD7816_STATUS_ERROR_WRONG_PARAMETERS_IN_DATA:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_WRONG_PARAMETERS_IN_DATA);

    case VCARD7816_STATUS_ERROR_FUNCTION_NOT_SUPPORTED:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_FUNCTION_NOT_SUPPORTED);

    case VCARD7816_STATUS_ERROR_FILE_NOT_FOUND:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_FILE_NOT_FOUND);

    case VCARD7816_STATUS_ERROR_RECORD_NOT_FOUND:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_RECORD_NOT_FOUND);

    case VCARD7816_STATUS_ERROR_NO_SPACE_FOR_FILE:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_NO_SPACE_FOR_FILE);

    case VCARD7816_STATUS_ERROR_LC_TLV_INCONSISTENT:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_LC_TLV_INCONSISTENT);

    case VCARD7816_STATUS_ERROR_P1_P2_INCORRECT:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_P1_P2_INCORRECT);

    case VCARD7816_STATUS_ERROR_LC_P1_P2_INCONSISTENT:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_LC_P1_P2_INCONSISTENT);

    case VCARD7816_STATUS_ERROR_DATA_NOT_FOUND:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_DATA_NOT_FOUND);

    case VCARD7816_STATUS_ERROR_WRONG_PARAMETERS_2:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_WRONG_PARAMETERS_2);

    case VCARD7816_STATUS_ERROR_INS_CODE_INVALID:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_INS_CODE_INVALID);

    case VCARD7816_STATUS_ERROR_CLA_INVALID:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_CLA_INVALID);

    case VCARD7816_STATUS_ERROR_GENERAL:

        return VCARD_RESPONSE_GET_STATIC(

                    VCARD7816_STATUS_ERROR_GENERAL);

    default:

        

        response = vcard_response_new_status(status);

        if (response == NULL) {

            

            return VCARD_RESPONSE_GET_STATIC(

                        VCARD7816_STATUS_EXC_ERROR_MEMORY_FAILURE);

        }

    }

    assert(response);

    return response;

}