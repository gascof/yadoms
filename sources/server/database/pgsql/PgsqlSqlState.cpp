#include "stdafx.h"
#include "PgsqlSqlState.h"
#include <shared/exception/OutOfRange.hpp>

namespace database { 
namespace pgsql { 

   DECLARE_ENUM_IMPLEMENTATION(ESqlErrorClass,
      ((SuccessfulCompletion)("00"))
      ((Warning)("01"))
      ((NoData)("02"))
      ((SQLStatementNotYetComplete)("03"))
      ((ConnectionException)("08"))
      ((TriggeredActionException)("09"))
      ((FeatureNotSupported)("0A"))
      ((InvalidTransactionInitiation)("0B"))
      ((LocatorException)("0F"))
      ((InvalidGrantor)("0L"))
      ((InvalidRoleSpecification)("0P"))
      ((CaseNotFound)("20"))
      ((CardinalityViolation)("21"))
      ((DataException)("22"))
      ((IntegrityConstraintViolation)("23"))
      ((InvalidCursorState)("24"))
      ((InvalidTransactionState)("25"))
      ((InvalidSQLStatementName)("26"))
      ((TriggeredDataChangeViolation)("27"))
      ((InvalidAuthorizationSpecification)("28"))
      ((DependentPrivilegeDescriptorsStillExist)("2B"))
      ((InvalidTransactionTermination)("2D"))
      ((SQLRoutineException)("2F"))
      ((InvalidCursorName)("34"))
      ((ExternalRoutineException)("38"))
      ((ExternalRoutineInvocationException)("39"))
      ((SavepointException)("3B"))
      ((InvalidCatalogName)("3D"))
      ((InvalidSchemaName)("3F"))
      ((TransactionRollback)("40"))
      ((SyntaxErrororAccessRuleViolation)("42"))
      ((WITHCHECKOPTIONViolation)("44"))
      ((InsufficientResources)("53"))
      ((ProgramLimitExceeded)("54"))
      ((ObjectNotInPrerequisiteState)("55"))
      ((OperatorIntervention)("57"))
      ((SystemError)("58"))
      ((ConfigurationFileError)("F0"))
      ((ForeignDataWrapperError)("HV"))
      ((PLpgSQLError)("P0"))
      ((InternalError)("XX"))
    )

   std::map<std::string, CPgsqlSqlState> CPgsqlSqlState::m_internalCodes = boost::assign::map_list_of
         ("00000", CPgsqlSqlState("00000", "successful_completion"))
         ("01000", CPgsqlSqlState("01000", "warning"))
         ("0100C", CPgsqlSqlState("0100C", "dynamic_result_sets_returned"))
         ("01008", CPgsqlSqlState("01008", "implicit_zero_bit_padding"))
         ("01003", CPgsqlSqlState("01003", "null_value_eliminated_in_set_function"))
         ("01007", CPgsqlSqlState("01007", "privilege_not_granted"))
         ("01006", CPgsqlSqlState("01006", "privilege_not_revoked"))
         ("01004", CPgsqlSqlState("01004", "string_data_right_truncation"))
         ("01P01", CPgsqlSqlState("01P01", "deprecated_feature"))
         ("02000", CPgsqlSqlState("02000", "no_data"))
         ("02001", CPgsqlSqlState("02001", "no_additional_dynamic_result_sets_returned"))
         ("03000", CPgsqlSqlState("03000", "sql_statement_not_yet_complete"))
         ("08000", CPgsqlSqlState("08000", "connection_exception"))
         ("08003", CPgsqlSqlState("08003", "connection_does_not_exist"))
         ("08006", CPgsqlSqlState("08006", "connection_failure"))
         ("08001", CPgsqlSqlState("08001", "sqlclient_unable_to_establish_sqlconnection"))
         ("08004", CPgsqlSqlState("08004", "sqlserver_rejected_establishment_of_sqlconnection"))
         ("08007", CPgsqlSqlState("08007", "transaction_resolution_unknown"))
         ("08P01", CPgsqlSqlState("08P01", "protocol_violation"))
         ("09000", CPgsqlSqlState("09000", "triggered_action_exception"))
         ("0A000", CPgsqlSqlState("0A000", "feature_not_supported"))
         ("0B000", CPgsqlSqlState("0B000", "invalid_transaction_initiation"))
         ("0F000", CPgsqlSqlState("0F000", "locator_exception"))
         ("0F001", CPgsqlSqlState("0F001", "invalid_locator_specification"))
         ("0L000", CPgsqlSqlState("0L000", "invalid_grantor"))
         ("0LP01", CPgsqlSqlState("0LP01", "invalid_grant_operation"))
         ("0P000", CPgsqlSqlState("0P000", "invalid_role_specification"))
         ("20000", CPgsqlSqlState("20000", "case_not_found"))
         ("21000", CPgsqlSqlState("21000", "cardinality_violation"))
         ("22000", CPgsqlSqlState("22000", "data_exception"))
         ("2202E", CPgsqlSqlState("2202E", "array_subscript_error"))
         ("22021", CPgsqlSqlState("22021", "character_not_in_repertoire"))
         ("22008", CPgsqlSqlState("22008", "datetime_field_overflow"))
         ("22012", CPgsqlSqlState("22012", "division_by_zero"))
         ("22005", CPgsqlSqlState("22005", "error_in_assignment"))
         ("2200B", CPgsqlSqlState("2200B", "escape_character_conflict"))
         ("22022", CPgsqlSqlState("22022", "indicator_overflow"))
         ("22015", CPgsqlSqlState("22015", "interval_field_overflow"))
         ("2201E", CPgsqlSqlState("2201E", "invalid_argument_for_logarithm"))
         ("22014", CPgsqlSqlState("22014", "invalid_argument_for_ntile_function"))
         ("22016", CPgsqlSqlState("22016", "invalid_argument_for_nth_value_function"))
         ("2201F", CPgsqlSqlState("2201F", "invalid_argument_for_power_function"))
         ("2201G", CPgsqlSqlState("2201G", "invalid_argument_for_width_bucket_function"))
         ("22018", CPgsqlSqlState("22018", "invalid_character_value_for_cast"))
         ("22007", CPgsqlSqlState("22007", "invalid_datetime_format"))
         ("22019", CPgsqlSqlState("22019", "invalid_escape_character"))
         ("2200D", CPgsqlSqlState("2200D", "invalid_escape_octet"))
         ("22025", CPgsqlSqlState("22025", "invalid_escape_sequence"))
         ("22P06", CPgsqlSqlState("22P06", "nonstandard_use_of_escape_character"))
         ("22010", CPgsqlSqlState("22010", "invalid_indicator_parameter_value"))
         ("22023", CPgsqlSqlState("22023", "invalid_parameter_value"))
         ("2201B", CPgsqlSqlState("2201B", "invalid_regular_expression"))
         ("2201W", CPgsqlSqlState("2201W", "invalid_row_count_in_limit_clause"))
         ("2201X", CPgsqlSqlState("2201X", "invalid_row_count_in_result_offset_clause"))
         ("22009", CPgsqlSqlState("22009", "invalid_time_zone_displacement_value"))
         ("2200C", CPgsqlSqlState("2200C", "invalid_use_of_escape_character"))
         ("2200G", CPgsqlSqlState("2200G", "most_specific_type_mismatch"))
         ("22004", CPgsqlSqlState("22004", "null_value_not_allowed"))
         ("22002", CPgsqlSqlState("22002", "null_value_no_indicator_parameter"))
         ("22003", CPgsqlSqlState("22003", "numeric_value_out_of_range"))
         ("22026", CPgsqlSqlState("22026", "string_data_length_mismatch"))
         ("22001", CPgsqlSqlState("22001", "string_data_right_truncation"))
         ("22011", CPgsqlSqlState("22011", "substring_error"))
         ("22027", CPgsqlSqlState("22027", "trim_error"))
         ("22024", CPgsqlSqlState("22024", "unterminated_c_string"))
         ("2200F", CPgsqlSqlState("2200F", "zero_length_character_string"))
         ("22P01", CPgsqlSqlState("22P01", "floating_point_exception"))
         ("22P02", CPgsqlSqlState("22P02", "invalid_text_representation"))
         ("22P03", CPgsqlSqlState("22P03", "invalid_binary_representation"))
         ("22P04", CPgsqlSqlState("22P04", "bad_copy_file_format"))
         ("22P05", CPgsqlSqlState("22P05", "untranslatable_character"))
         ("2200L", CPgsqlSqlState("2200L", "not_an_xml_document"))
         ("2200M", CPgsqlSqlState("2200M", "invalid_xml_document"))
         ("2200N", CPgsqlSqlState("2200N", "invalid_xml_content"))
         ("2200S", CPgsqlSqlState("2200S", "invalid_xml_comment"))
         ("2200T", CPgsqlSqlState("2200T", "invalid_xml_processing_instruction"))
         ("23000", CPgsqlSqlState("23000", "integrity_constraint_violation"))
         ("23001", CPgsqlSqlState("23001", "restrict_violation"))
         ("23502", CPgsqlSqlState("23502", "not_null_violation"))
         ("23503", CPgsqlSqlState("23503", "foreign_key_violation"))
         ("23505", CPgsqlSqlState("23505", "unique_violation"))
         ("23514", CPgsqlSqlState("23514", "check_violation"))
         ("23P01", CPgsqlSqlState("23P01", "exclusion_violation"))
         ("24000", CPgsqlSqlState("24000", "invalid_cursor_state"))
         ("25000", CPgsqlSqlState("25000", "invalid_transaction_state"))
         ("25001", CPgsqlSqlState("25001", "active_sql_transaction"))
         ("25002", CPgsqlSqlState("25002", "branch_transaction_already_active"))
         ("25008", CPgsqlSqlState("25008", "held_cursor_requires_same_isolation_level"))
         ("25003", CPgsqlSqlState("25003", "inappropriate_access_mode_for_branch_transaction"))
         ("25004", CPgsqlSqlState("25004", "inappropriate_isolation_level_for_branch_transaction"))
         ("25005", CPgsqlSqlState("25005", "no_active_sql_transaction_for_branch_transaction"))
         ("25006", CPgsqlSqlState("25006", "read_only_sql_transaction"))
         ("25007", CPgsqlSqlState("25007", "schema_and_data_statement_mixing_not_supported"))
         ("25P01", CPgsqlSqlState("25P01", "no_active_sql_transaction"))
         ("25P02", CPgsqlSqlState("25P02", "in_failed_sql_transaction"))
         ("26000", CPgsqlSqlState("26000", "invalid_sql_statement_name"))
         ("27000", CPgsqlSqlState("27000", "triggered_data_change_violation"))
         ("28000", CPgsqlSqlState("28000", "invalid_authorization_specification"))
         ("28P01", CPgsqlSqlState("28P01", "invalid_password"))
         ("2B000", CPgsqlSqlState("2B000", "dependent_privilege_descriptors_still_exist"))
         ("2BP01", CPgsqlSqlState("2BP01", "dependent_objects_still_exist"))
         ("2D000", CPgsqlSqlState("2D000", "invalid_transaction_termination"))
         ("2F000", CPgsqlSqlState("2F000", "sql_routine_exception"))
         ("2F005", CPgsqlSqlState("2F005", "function_executed_no_return_statement"))
         ("2F002", CPgsqlSqlState("2F002", "modifying_sql_data_not_permitted"))
         ("2F003", CPgsqlSqlState("2F003", "prohibited_sql_statement_attempted"))
         ("2F004", CPgsqlSqlState("2F004", "reading_sql_data_not_permitted"))
         ("34000", CPgsqlSqlState("34000", "invalid_cursor_name"))
         ("38000", CPgsqlSqlState("38000", "external_routine_exception"))
         ("38001", CPgsqlSqlState("38001", "containing_sql_not_permitted"))
         ("38002", CPgsqlSqlState("38002", "modifying_sql_data_not_permitted"))
         ("38003", CPgsqlSqlState("38003", "prohibited_sql_statement_attempted"))
         ("38004", CPgsqlSqlState("38004", "reading_sql_data_not_permitted"))
         ("39000", CPgsqlSqlState("39000", "external_routine_invocation_exception"))
         ("39001", CPgsqlSqlState("39001", "invalid_sqlstate_returned"))
         ("39004", CPgsqlSqlState("39004", "null_value_not_allowed"))
         ("39P01", CPgsqlSqlState("39P01", "trigger_protocol_violated"))
         ("39P02", CPgsqlSqlState("39P02", "srf_protocol_violated"))
         ("3B000", CPgsqlSqlState("3B000", "savepoint_exception"))
         ("3B001", CPgsqlSqlState("3B001", "invalid_savepoint_specification"))
         ("3D000", CPgsqlSqlState("3D000", "invalid_catalog_name"))
         ("3F000", CPgsqlSqlState("3F000", "invalid_schema_name"))
         ("40000", CPgsqlSqlState("40000", "transaction_rollback"))
         ("40002", CPgsqlSqlState("40002", "transaction_integrity_constraint_violation"))
         ("40001", CPgsqlSqlState("40001", "serialization_failure"))
         ("40003", CPgsqlSqlState("40003", "statement_completion_unknown"))
         ("40P01", CPgsqlSqlState("40P01", "deadlock_detected"))
         ("42000", CPgsqlSqlState("42000", "syntax_error_or_access_rule_violation"))
         ("42601", CPgsqlSqlState("42601", "syntax_error"))
         ("42501", CPgsqlSqlState("42501", "insufficient_privilege"))
         ("42846", CPgsqlSqlState("42846", "cannot_coerce"))
         ("42803", CPgsqlSqlState("42803", "grouping_error"))
         ("42P20", CPgsqlSqlState("42P20", "windowing_error"))
         ("42P19", CPgsqlSqlState("42P19", "invalid_recursion"))
         ("42830", CPgsqlSqlState("42830", "invalid_foreign_key"))
         ("42602", CPgsqlSqlState("42602", "invalid_name"))
         ("42622", CPgsqlSqlState("42622", "name_too_long"))
         ("42939", CPgsqlSqlState("42939", "reserved_name"))
         ("42804", CPgsqlSqlState("42804", "datatype_mismatch"))
         ("42P18", CPgsqlSqlState("42P18", "indeterminate_datatype"))
         ("42P21", CPgsqlSqlState("42P21", "collation_mismatch"))
         ("42P22", CPgsqlSqlState("42P22", "indeterminate_collation"))
         ("42809", CPgsqlSqlState("42809", "wrong_object_type"))
         ("42703", CPgsqlSqlState("42703", "undefined_column"))
         ("42883", CPgsqlSqlState("42883", "undefined_function"))
         ("42P01", CPgsqlSqlState("42P01", "undefined_table"))
         ("42P02", CPgsqlSqlState("42P02", "undefined_parameter"))
         ("42704", CPgsqlSqlState("42704", "undefined_object"))
         ("42701", CPgsqlSqlState("42701", "duplicate_column"))
         ("42P03", CPgsqlSqlState("42P03", "duplicate_cursor"))
         ("42P04", CPgsqlSqlState("42P04", "duplicate_database"))
         ("42723", CPgsqlSqlState("42723", "duplicate_function"))
         ("42P05", CPgsqlSqlState("42P05", "duplicate_prepared_statement"))
         ("42P06", CPgsqlSqlState("42P06", "duplicate_schema"))
         ("42P07", CPgsqlSqlState("42P07", "duplicate_table"))
         ("42712", CPgsqlSqlState("42712", "duplicate_alias"))
         ("42710", CPgsqlSqlState("42710", "duplicate_object"))
         ("42702", CPgsqlSqlState("42702", "ambiguous_column"))
         ("42725", CPgsqlSqlState("42725", "ambiguous_function"))
         ("42P08", CPgsqlSqlState("42P08", "ambiguous_parameter"))
         ("42P09", CPgsqlSqlState("42P09", "ambiguous_alias"))
         ("42P10", CPgsqlSqlState("42P10", "invalid_column_reference"))
         ("42611", CPgsqlSqlState("42611", "invalid_column_definition"))
         ("42P11", CPgsqlSqlState("42P11", "invalid_cursor_definition"))
         ("42P12", CPgsqlSqlState("42P12", "invalid_database_definition"))
         ("42P13", CPgsqlSqlState("42P13", "invalid_function_definition"))
         ("42P14", CPgsqlSqlState("42P14", "invalid_prepared_statement_definition"))
         ("42P15", CPgsqlSqlState("42P15", "invalid_schema_definition"))
         ("42P16", CPgsqlSqlState("42P16", "invalid_table_definition"))
         ("42P17", CPgsqlSqlState("42P17", "invalid_object_definition"))
         ("44000", CPgsqlSqlState("44000", "with_check_option_violation"))
         ("53000", CPgsqlSqlState("53000", "insufficient_resources"))
         ("53100", CPgsqlSqlState("53100", "disk_full"))
         ("53200", CPgsqlSqlState("53200", "out_of_memory"))
         ("53300", CPgsqlSqlState("53300", "too_many_connections"))
         ("54000", CPgsqlSqlState("54000", "program_limit_exceeded"))
         ("54001", CPgsqlSqlState("54001", "statement_too_complex"))
         ("54011", CPgsqlSqlState("54011", "too_many_columns"))
         ("54023", CPgsqlSqlState("54023", "too_many_arguments"))
         ("55000", CPgsqlSqlState("55000", "object_not_in_prerequisite_state"))
         ("55006", CPgsqlSqlState("55006", "object_in_use"))
         ("55P02", CPgsqlSqlState("55P02", "cant_change_runtime_param"))
         ("55P03", CPgsqlSqlState("55P03", "lock_not_available"))
         ("57000", CPgsqlSqlState("57000", "operator_intervention"))
         ("57014", CPgsqlSqlState("57014", "query_canceled"))
         ("57P01", CPgsqlSqlState("57P01", "admin_shutdown"))
         ("57P02", CPgsqlSqlState("57P02", "crash_shutdown"))
         ("57P03", CPgsqlSqlState("57P03", "cannot_connect_now"))
         ("57P04", CPgsqlSqlState("57P04", "database_dropped"))
         ("58030", CPgsqlSqlState("58030", "io_error"))
         ("58P01", CPgsqlSqlState("58P01", "undefined_file"))
         ("58P02", CPgsqlSqlState("58P02", "duplicate_file"))
         ("F0000", CPgsqlSqlState("F0000", "config_file_error"))
         ("F0001", CPgsqlSqlState("F0001", "lock_file_exists"))
         ("HV000", CPgsqlSqlState("HV000", "fdw_error"))
         ("HV005", CPgsqlSqlState("HV005", "fdw_column_name_not_found"))
         ("HV002", CPgsqlSqlState("HV002", "fdw_dynamic_parameter_value_needed"))
         ("HV010", CPgsqlSqlState("HV010", "fdw_function_sequence_error"))
         ("HV021", CPgsqlSqlState("HV021", "fdw_inconsistent_descriptor_information"))
         ("HV024", CPgsqlSqlState("HV024", "fdw_invalid_attribute_value"))
         ("HV007", CPgsqlSqlState("HV007", "fdw_invalid_column_name"))
         ("HV008", CPgsqlSqlState("HV008", "fdw_invalid_column_number"))
         ("HV004", CPgsqlSqlState("HV004", "fdw_invalid_data_type"))
         ("HV006", CPgsqlSqlState("HV006", "fdw_invalid_data_type_descriptors"))
         ("HV091", CPgsqlSqlState("HV091", "fdw_invalid_descriptor_field_identifier"))
         ("HV00B", CPgsqlSqlState("HV00B", "fdw_invalid_handle"))
         ("HV00C", CPgsqlSqlState("HV00C", "fdw_invalid_option_index"))
         ("HV00D", CPgsqlSqlState("HV00D", "fdw_invalid_option_name"))
         ("HV090", CPgsqlSqlState("HV090", "fdw_invalid_string_length_or_buffer_length"))
         ("HV00A", CPgsqlSqlState("HV00A", "fdw_invalid_string_format"))
         ("HV009", CPgsqlSqlState("HV009", "fdw_invalid_use_of_null_pointer"))
         ("HV014", CPgsqlSqlState("HV014", "fdw_too_many_handles"))
         ("HV001", CPgsqlSqlState("HV001", "fdw_out_of_memory"))
         ("HV00P", CPgsqlSqlState("HV00P", "fdw_no_schemas"))
         ("HV00J", CPgsqlSqlState("HV00J", "fdw_option_name_not_found"))
         ("HV00K", CPgsqlSqlState("HV00K", "fdw_reply_handle"))
         ("HV00Q", CPgsqlSqlState("HV00Q", "fdw_schema_not_found"))
         ("HV00R", CPgsqlSqlState("HV00R", "fdw_table_not_found"))
         ("HV00L", CPgsqlSqlState("HV00L", "fdw_unable_to_create_execution"))
         ("HV00M", CPgsqlSqlState("HV00M", "fdw_unable_to_create_reply"))
         ("HV00N", CPgsqlSqlState("HV00N", "fdw_unable_to_establish_connection"))
         ("P0000", CPgsqlSqlState("P0000", "plpgsql_error"))
         ("P0001", CPgsqlSqlState("P0001", "raise_exception"))
         ("P0002", CPgsqlSqlState("P0002", "no_data_found"))
         ("P0003", CPgsqlSqlState("P0003", "too_many_rows"))
         ("XX000", CPgsqlSqlState("XX000", "internal_error"))
         ("XX001", CPgsqlSqlState("XX001", "data_corrupted"))
         ("XX002", CPgsqlSqlState("XX002", "index_corrupted"));


   CPgsqlSqlState::CPgsqlSqlState(const std::string & code, const std::string & description)
      :m_code(code), m_description(description)
   {
   }

   CPgsqlSqlState::~CPgsqlSqlState()
   {

   }

   const std::string & CPgsqlSqlState::GetCode() const
   {
      return m_code;
   }

   const std::string & CPgsqlSqlState::GetDescription() const
   {
      return m_description;
   }

   const ESqlErrorClass CPgsqlSqlState::GetClass() const
   {
      return ESqlErrorClass(m_code.substr(0, 2));
   }  
   
   const CPgsqlSqlState & CPgsqlSqlState::Parse(char * state)
   {
      std::map<std::string, CPgsqlSqlState>::iterator find = m_internalCodes.find(state);
      if (find != m_internalCodes.end())
         return find->second;
      throw shared::exception::COutOfRange("PgsqlState not found");
   }


   

} //namespace pgsql
} //namespace database 

