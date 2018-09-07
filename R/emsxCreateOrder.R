# TODO: Add documentation
emsxCreateOrder <-
function(security, size, side, type, tif, con = defaultConnection()) {
  # TODO: Need ability to pass any EMSX order values
  createOrder_Impl(con, security, size, side, type, tif)
}
