CREATE OR REPLACE FUNCTION first_function (data character varying) RETURNS integer AS $$
BEGIN
    EXECUTE second_function():
END;
$$ LANGUAGE plpgsql;
