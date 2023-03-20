package main

import (

	"github.com/envoyproxy/envoy/contrib/golang/filters/http/source/go/pkg/api"
	"google.golang.org/protobuf/types/known/structpb"
)

var UpdateUpstreamBody = "upstream response body updated by the simple plugin"

type filter struct {
	callbacks api.FilterCallbackHandler
	path      string
	config    *structpb.Struct
}

func (f *filter) DecodeHeaders(header api.RequestHeaderMap, endStream bool) api.StatusType {
	prefix := "test"

	for i := 0; i < 20; i++ {
		header.Set(prefix, "hahahaha")
	}

	for i := 0; i < 30; i++ {
		val, _ := header.Get(prefix)
		if val != "hahahaha" {
			println("header mistach!")
		}
	}

	return api.Continue
}

func (f *filter) DecodeData(buffer api.BufferInstance, endStream bool) api.StatusType {
	return api.Continue
}

func (f *filter) DecodeTrailers(trailers api.RequestTrailerMap) api.StatusType {
	return api.Continue
}

func (f *filter) EncodeHeaders(header api.ResponseHeaderMap, endStream bool) api.StatusType {
	return api.Continue
}

func (f *filter) EncodeData(buffer api.BufferInstance, endStream bool) api.StatusType {
	for i := 0; i < 30; i++ {
		buffer.Bytes()
	}
	return api.Continue
}

func (f *filter) EncodeTrailers(trailers api.ResponseTrailerMap) api.StatusType {
	return api.Continue
}

func (f *filter) OnDestroy(reason api.DestroyReason) {
}
